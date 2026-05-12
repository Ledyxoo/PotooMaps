#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "uniStr.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// +----------------+----------+----------+----------+----------+
// | Valeur         | Octet 1  | Octet 2  | Octet 3  | Octet 4  |
// +----------------+----------+----------+----------+----------+
// |  0000 -   007F | 0xxxxxxx |          |          |          |
// |  0080 -   07FF | 110xxxxx | 10xxxxxx |          |          |
// |  0800 -   FFFF | 1110xxxx | 10xxxxxx | 10xxxxxx |          |
// | 10000 - 10FFFF | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
// +----------------+----------+----------+----------+----------+

/// @brief Renvoie le nombre de caractères dans une chaîne codée en UTF8.
/// @param bufferU8 la chaîne codée en UTF8.
/// @param bufferSize la taille du buffer en octets.
/// @return Le nombre de caractères, -1 en cas d'erreur.
static int getSizeU8To32(const char *bufferU8, int bufferSize)
{
    int sizeU32 = 0;
    int i = 0;
    while ((i < bufferSize) && (bufferU8[i] != '\0'))
    {
        char c = bufferU8[i];
        if ((c & 0x80) == 0x00)
        {
            // 1 octet
            i++; sizeU32++;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            // 2 octets
            if (i + 1 >= bufferSize) return -1;
            if ((bufferU8[i + 1] & 0xC0) != 0x80) return -1;
            i += 2; sizeU32++;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            // 3 octets
            if (i + 2 >= bufferSize) return -1;
            if ((bufferU8[i + 1] & 0xC0) != 0x80) return -1;
            if ((bufferU8[i + 2] & 0xC0) != 0x80) return -1;
            i += 3; sizeU32++;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            // 4 octets
            if (i + 3 >= bufferSize) return -1;
            if ((bufferU8[i + 1] & 0xC0) != 0x80) return -1;
            if ((bufferU8[i + 2] & 0xC0) != 0x80) return -1;
            if ((bufferU8[i + 3] & 0xC0) != 0x80) return -1;
            i += 4; sizeU32++;
        }
        else return -1;
    }
    if (i >= bufferSize)
        return -1;
    return sizeU32;
}

/// @brief Renvoie le nombre d'octets nécessaires pour coder une chaîne en UTF8.
/// @param buffer 
/// @param bufferSize 
/// @return 
static int getSizeU32ToU8(char32 *buffer, int bufferSize)
{
    int sizeU8 = 0;
    for (int i = 0; i < bufferSize; i++)
    {
        char32 c = buffer[i];
        if      (c <    0x80) sizeU8 += 1;
        else if (c <   0x800) sizeU8 += 2;
        else if (c < 0x10000) sizeU8 += 3;
        else                  sizeU8 += 4;
    }
    return sizeU8;
}

// Fonction de la librairie cute_utf
static char* decodeUTF8(char* text, char32* cp)
{
    unsigned char c = *text++;
    unsigned int min = 0;
    int extra = 0;
    *cp = 0;
    if      (c >= 0xF0) { *cp = c & 0x07; extra = 3; min = 0x10000; }
    else if (c >= 0xE0) { *cp = c & 0x0F; extra = 2; min = 0x800; }
    else if (c >= 0xC0) { *cp = c & 0x1F; extra = 1; min = 0x80; }
    else if (c >= 0x80) { *cp = 0xFFFD; }
    else *cp = c;
    while (extra--)
    {
        c = *text++;
        if ((c & 0xC0) != 0x80) { *cp = 0xFFFD; break; }
        (*cp) = ((*cp) << 6) | (c & 0x3F);
    }
    if (*cp < min) *cp = 0xFFFD;
    return text;
}

// Fonction de la librairie cute_utf
static char *encodeUTF8(char *text, char32 cp)
{
    if (cp > 0x10FFFF) cp = 0xFFFD;

#define EMIT(X, Y, Z) *text++ = X | ((cp >> Y) & Z)

    if (cp < 0x80)
    {
        EMIT(0x00, 0, 0x7F);
    }
    else if (cp < 0x800)
    {
        EMIT(0xC0, 6, 0x1F);
        EMIT(0x80, 0, 0x3F);
    }
    else if (cp < 0x10000)
    {
        EMIT(0xE0, 12, 0xF);
        EMIT(0x80, 6, 0x3F);
        EMIT(0x80, 0, 0x3F);
    }
    else
    {
        EMIT(0xF0, 18, 0x7);
        EMIT(0x80, 12, 0x3F);
        EMIT(0x80, 6, 0x3F);
        EMIT(0x80, 0, 0x3F);
    }
    return text;
}
#undef EMIT

UniStr *UniStr_decodeU8(char *bufferU8, int bufferSize)
{
    if (bufferSize < 0)
    {
        bufferSize = INT_MAX;
    }

    int sizeU32 = getSizeU8To32(bufferU8, bufferSize);
    if (sizeU32 < 0)
    {
        assert(false);
        return NULL;
    }

    UniStr *string = (UniStr *)calloc(1, sizeof(UniStr));
    AssertNew(string);

    char32 *data = NULL;
    if (sizeU32 > 0)
    {
        data = (char32 *)calloc(sizeU32, sizeof(char32));
        AssertNew(data);

        for (int i = 0; i < sizeU32; i++)
            bufferU8 = decodeUTF8(bufferU8, data + i);
    }

    string->data = data;
    string->length = sizeU32;

    return string;
}

UniStr *UniStr_decodeAscii(char *bufferAscii, int bufferSize)
{
    if (bufferSize < 0)
    {
        bufferSize = INT_MAX;
    }

    int sizeU32 = getSizeU8To32(bufferAscii, bufferSize);
    if (sizeU32 < 0)
    {
        assert(false);
        return NULL;
    }

    UniStr *string = (UniStr *)calloc(1, sizeof(UniStr));
    AssertNew(string);

    char32 *data = NULL;
    if (sizeU32 > 0)
    {
        data = (char32 *)calloc(sizeU32, sizeof(char32));
        AssertNew(data);

        for (int i = 0; i < sizeU32; i++)
            data[i] = (char32)bufferAscii[i];
    }

    string->data = data;
    string->length = sizeU32;

    return string;
}

static UniStr *UniStr_create(int length)
{
    UniStr *string = (UniStr *)calloc(1, sizeof(UniStr));
    AssertNew(string);

    char32 *data = NULL;
    if (length > 0)
    {
        data = (char32 *)calloc(length, sizeof(char32));
        AssertNew(data);
    }

    string->length = length;
    string->data = data;

    return string;
}

void UniStr_destroy(UniStr *string)
{
    if (!string) return;

    free(string->data);
    free(string);
}

void UniStr_destroyArray(UniStr **strings, int count)
{
    if (!strings) return;
    for (int i = 0; i < count; i++)
    {
        UniStr_destroy(strings[i]);
    }
    free(strings);
}

char *UniStr_encodeU8(UniStr *string)
{
    char32 *data = string->data;
    int sizeU32 = string->length;
    int sizeU8 = getSizeU32ToU8(data, sizeU32);

    char *bufferU8 = (char *)calloc(sizeU8 + 1, sizeof(char));
    AssertNew(bufferU8);

    char *ptr = bufferU8;
    for (int i = 0; i < sizeU32; i++)
    {
        ptr = encodeUTF8(ptr, data[i]);
    }

    return bufferU8;
}

//         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
// ------------------------------------------------------
// U+00C0  À  Á  Â  Ã  Ä  Å  Æ  Ç  È  É  Ê  Ë  Ì  Í  Î  Ï
// U+00D0  Ð  Ñ  Ò  Ó  Ô  Õ  Ö  ×  Ø  Ù  Ú  Û  Ü  Ý  Þ  ß
// U+00E0  à  á  â  ã  ä  å  æ  ç  è  é  ê  ë  ì  í  î  ï
// U+00F0  ð  ñ  ò  ó  ô  õ  ö  ÷  ø  ù  ú  û  ü  ý  þ  ÿ
// 
// U+0150  Ő  ő  Œ  œ  Ŕ  ŕ  Ŗ  ŗ  Ř  ř  Ś  ś  Ŝ  ŝ  Ş  ş

char *UniStr_encodeAscii(UniStr *string)
{
    char32 *data = string->data;
    int sizeU32 = string->length;
    int sizeAscii = sizeU32;
    for (int i = 0; i < sizeU32; i++)
    {
        int c = (int)(data[i]);
        switch (c)
        {
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00E0  à  á  â  ã  ä  å  æ  ç  è  é  ê  ë  ì  í  î  ï
            // U+0150  Ő  ő  Œ  œ  Ŕ  ŕ  Ŗ  ŗ  Ř  ř  Ś  ś  Ŝ  ŝ  Ş  ş
        case 0xC6: case 0xE6: case 0x152: case 0x153:
            // AE ae OE oe
            sizeAscii++;
            break;
        default:
            break;
        }
    }
    char *bufferAscii = (char *)calloc(sizeAscii + 1, sizeof(char));
    AssertNew(bufferAscii);

    int idx = 0;
    for (int i = 0; i < sizeU32; i++)
    {
        int c = (int)(data[i]);
        if (c < 128)
        {
            bufferAscii[idx++] = (char)c;
            continue;
        }
        switch (c)
        {
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00C0  À  Á  Â  Ã  Ä  Å  Æ  Ç  È  É  Ê  Ë  Ì  Í  Î  Ï
        case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5:
            bufferAscii[idx++] = 'A';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00C0  À  Á  Â  Ã  Ä  Å  Æ  Ç  È  É  Ê  Ë  Ì  Í  Î  Ï
        case 0xC6:
            bufferAscii[idx++] = 'A';
            bufferAscii[idx++] = 'E';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00C0  À  Á  Â  Ã  Ä  Å  Æ  Ç  È  É  Ê  Ë  Ì  Í  Î  Ï
        case 0xC7:
            bufferAscii[idx++] = 'C';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00C0  À  Á  Â  Ã  Ä  Å  Æ  Ç  È  É  Ê  Ë  Ì  Í  Î  Ï
        case 0xC8: case 0xC9: case 0xCA: case 0xCB:
            bufferAscii[idx++] = 'E';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00C0  À  Á  Â  Ã  Ä  Å  Æ  Ç  È  É  Ê  Ë  Ì  Í  Î  Ï
        case 0xCC: case 0xCD: case 0xCE: case 0xCF:
            bufferAscii[idx++] = 'I';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00D0  Ð  Ñ  Ò  Ó  Ô  Õ  Ö  ×  Ø  Ù  Ú  Û  Ü  Ý  Þ  ß
        case 0xD0:
            bufferAscii[idx++] = 'D';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00D0  Ð  Ñ  Ò  Ó  Ô  Õ  Ö  ×  Ø  Ù  Ú  Û  Ü  Ý  Þ  ß
        case 0xD1:
            bufferAscii[idx++] = 'N';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00D0  Ð  Ñ  Ò  Ó  Ô  Õ  Ö  ×  Ø  Ù  Ú  Û  Ü  Ý  Þ  ß
        case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6:
            bufferAscii[idx++] = 'O';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00D0  Ð  Ñ  Ò  Ó  Ô  Õ  Ö  ×  Ø  Ù  Ú  Û  Ü  Ý  Þ  ß
        case 0xD9: case 0xDA: case 0xDB: case 0xDC:
            bufferAscii[idx++] = 'U';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00D0  Ð  Ñ  Ò  Ó  Ô  Õ  Ö  ×  Ø  Ù  Ú  Û  Ü  Ý  Þ  ß
        case 0xDE:
            bufferAscii[idx++] = 'Y';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00E0  à  á  â  ã  ä  å  æ  ç  è  é  ê  ë  ì  í  î  ï
        case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5:
            bufferAscii[idx++] = 'a';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00E0  à  á  â  ã  ä  å  æ  ç  è  é  ê  ë  ì  í  î  ï
        case 0xE6:
            bufferAscii[idx++] = 'a';
            bufferAscii[idx++] = 'e';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00E0  à  á  â  ã  ä  å  æ  ç  è  é  ê  ë  ì  í  î  ï
        case 0xE7:
            bufferAscii[idx++] = 'c';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00E0  à  á  â  ã  ä  å  æ  ç  è  é  ê  ë  ì  í  î  ï
        case 0xE8: case 0xE9: case 0xEA: case 0xEB:
            bufferAscii[idx++] = 'e';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00E0  à  á  â  ã  ä  å  æ  ç  è  é  ê  ë  ì  í  î  ï
        case 0xEC: case 0xED: case 0xEE: case 0xEF:
            bufferAscii[idx++] = 'i';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00F0  ð  ñ  ò  ó  ô  õ  ö  ÷  ø  ù  ú  û  ü  ý  þ  ÿ
        case 0xF0:
            bufferAscii[idx++] = 'd';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00F0  ð  ñ  ò  ó  ô  õ  ö  ÷  ø  ù  ú  û  ü  ý  þ  ÿ
        case 0xF1:
            bufferAscii[idx++] = 'n';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00F0  ð  ñ  ò  ó  ô  õ  ö  ÷  ø  ù  ú  û  ü  ý  þ  ÿ
        case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6:
            bufferAscii[idx++] = 'o';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00F0  ð  ñ  ò  ó  ô  õ  ö  ÷  ø  ù  ú  û  ü  ý  þ  ÿ
        case 0xF9: case 0xFA: case 0xFB: case 0xFC:
            bufferAscii[idx++] = 'u';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+00F0  ð  ñ  ò  ó  ô  õ  ö  ÷  ø  ù  ú  û  ü  ý  þ  ÿ
        case 0xFE: case 0xFF:
            bufferAscii[idx++] = 'y';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+0150  Ő  ő  Œ  œ  Ŕ  ŕ  Ŗ  ŗ  Ř  ř  Ś  ś  Ŝ  ŝ  Ş  ş
        case 0x152:
            bufferAscii[idx++] = 'O';
            bufferAscii[idx++] = 'E';
            break;
            //         0  1  2  3  4  5  6  7  8  9  A  B  C  E  D  F
            // U+0150  Ő  ő  Œ  œ  Ŕ  ŕ  Ŗ  ŗ  Ř  ř  Ś  ś  Ŝ  ŝ  Ş  ş
        case 0x153:
            bufferAscii[idx++] = 'o';
            bufferAscii[idx++] = 'e';
            break;
        case 0xB4: case 0x2018: case 0x2019: case 0x201A: case 0x201B:
        case 0x2032: case 0x2035:
            bufferAscii[idx++] = '\'';
            break;
        case 0x201C: case 0x201D: case 0x201E: case 0x201F:
        case 0x2033: case 0x2036:
            bufferAscii[idx++] = '\"';
            break;
        default:
            bufferAscii[idx++] = '?';
            break;
        }
    }

    return bufferAscii;
}

void UniStr_print(UniStr *string)
{
    if (!string) return;

    if (string->data == NULL)
    {
        printf("()");
        return;
    }
    char *buffer = UniStr_encodeAscii(string);
    printf("%s", buffer);
    free(buffer);
}

static bool UniStr_isSubAt(UniStr *string, UniStr *sub, int index)
{
    char32 *strData = string->data;
    char32 *subData = sub->data;
    int strLen = string->length;
    int subLen = sub->length;

    if (index < 0 || index + subLen >= strLen)
        return false;

    for (int i = 0; i < subLen; i++)
    {
        if (strData[index + i] != subData[i])
            return false;
    }
    return true;
}

/// @brief 
/// @param string 
/// @param start indice de début (inclu)
/// @param end indice de fin (exclu)
/// @return 
UniStr *UniStr_slice(UniStr *string, int start, int end)
{
    if (start < 0) start = 0;
    if (end > string->length) end = string->length;

    int subLen = end - start;
    if (subLen <= 0)
    {
        return UniStr_create(0);
    }

    UniStr *sub = (UniStr *)calloc(1, sizeof(UniStr));
    AssertNew(sub);

    sub->length = subLen;
    sub->data = (char32 *)calloc(subLen + 1, sizeof(char32));
    AssertNew(sub->data);

    Memcpy(
        sub->data, subLen * sizeof(char32),
        string->data + start, subLen * sizeof(char32)
    );

    return sub;
}

int UniStr_count(UniStr *string, UniStr *sub)
{
    int count = 0;
    int strLen = string->length;
    for (int i = 0; i < strLen; i++)
    {
        if (UniStr_isSubAt(string, sub, i))
        {
            count++;
        }
    }
    return count;
}

int UniStr_find(UniStr *string, UniStr *sub)
{
    int strLen = string->length;
    for (int i = 0; i < strLen; i++)
    {
        if (UniStr_isSubAt(string, sub, i))
        {
            return i;
        }
    }
    return -1;
}

int UniStr_rfind(UniStr *string, UniStr *sub)
{
    for (int i = string->length - sub->length; i >= 0; i--)
    {
        if (UniStr_isSubAt(string, sub, i))
        {
            return i;
        }
    }
    return -1;
}

UniStr **UniStr_split(UniStr *string, UniStr *sep, int *subCount)
{
    /*char32 *strData = string->data;
    char32 *sepData = sep->data;*/
    int strLen = string->length;
    int sepLen = sep->length;

    int count = 1;
    for (int i = 0; i < strLen; )
    {
        if (UniStr_isSubAt(string, sep, i))
        {
            count++;
            i += sepLen;
        }
        else i++;
    }

    UniStr **subStrings = (UniStr **)calloc(count, sizeof(UniStr *));
    AssertNew(subStrings);

    int subIdx = 0;
    int start = 0;
    for (int i = 0; i < strLen; )
    {
        if (UniStr_isSubAt(string, sep, i))
        {
            subStrings[subIdx++] = UniStr_slice(string, start, i);
            i += sepLen;
            start = i;
        }
        else i++;
    }
    subStrings[subIdx++] = UniStr_slice(string, start, strLen);

    *subCount = count;
    return subStrings;
}

UniStr *UniStr_join(UniStr *string1, UniStr *string2)
{
    assert(string1 && string2);

    int strLen1 = string1->length;
    int strLen2 = string2->length;

    UniStr *res = UniStr_create(strLen1 + strLen2);
    AssertNew(res);

    Memcpy(
        res->data, strLen1 * sizeof(char32),
        string1->data, strLen1 * sizeof(char32)
    );
    Memcpy(
        res->data + strLen1, strLen2 * sizeof(char32),
        string2->data, strLen2 * sizeof(char32)
    );

    return res;
}

UniStr *UniStr_copy(UniStr *string)
{
    int strLen = string->length;
    UniStr *res = UniStr_create(strLen);
    Memcpy(
        res->data, strLen * sizeof(char32),
        string->data, strLen * sizeof(char32)
    );
    res->length = strLen;
    return res;
}

static char *UniStr_getNumberStr(UniStr *string, int *start)
{
    int maxLen = 64;
    char *res = (char *)calloc(maxLen + 1, sizeof(char));
    AssertNew(res);

    int i = *start;
    for (; i < string->length; i++)
    {
        bool isSpace = false;
        switch (string->data[i])
        {
        case ' ': case '\t': case '\n': case '\r': case '\v': case '\f':
            isSpace = true;
        default:
            break;
        }

        if (!isSpace) break;
    }
    *start = i;

    int resIdx = 0;
    for (; i < string->length; i++)
    {
        bool isNumber = false;
        switch (string->data[i])
        {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '+': case '-': case 'e': case 'E': case '.':
            isNumber = true;
            break;
        default:
            break;
        }
        
        if (isNumber && resIdx < maxLen)
        {
            res[resIdx++] = (char)string->data[i];
        }
        else break;
    }
    res[resIdx++] = '\0';

    return res;
}

int UniStr_getInt(UniStr *string, int start, int *end)
{
    char *buffer = UniStr_getNumberStr(string, &start);
    char *endPtr = NULL;
    long value = strtol(buffer, &endPtr, 10);
    if (end) *end = start + (int)(endPtr - buffer);
    free(buffer);
    return (int)value;
}

double UniStr_getFloat(UniStr *string, int start, int *end)
{
    char *buffer = UniStr_getNumberStr(string, &start);
    char *endPtr = NULL;
    float value = strtof(buffer, &endPtr);
    if (end) *end = start + (int)(endPtr - buffer);
    free(buffer);
    return value;
}

double UniStr_getDouble(UniStr *string, int start, int *end)
{
    char *buffer = UniStr_getNumberStr(string, &start);
    char *endPtr = NULL;
    double value = strtod(buffer, &endPtr);
    if (end) *end = start + (int)(endPtr - buffer);
    free(buffer);
    return value;
}

unsigned long long UniStr_getULL(UniStr *string, int start, int *end)
{
    char *buffer = UniStr_getNumberStr(string, &start);
    char *endPtr = NULL;
    unsigned long long value = strtoull(buffer, &endPtr, 10);
    if (end) *end = start + (int)(endPtr - buffer);
    free(buffer);
    return value;
}

bool UniStr_isEmpty(UniStr *string)
{
    return (string->length <= 0);
}

/// @brief Extrait une sous-chaîne délimitée par (), {} ou [].
/// @param string 
/// @param index 
/// @return 
UniStr *UniStr_getBlock(UniStr *string, int index)
{
    char32 oDelimiters[3] = { '(', '{', '[' };
    char32 cDelimiters[3] = { '(', '}', ']' };
    int delIdx = -1;

    char32 *data = string->data;
    int start, strLen = string->length;
    for (start = index; delIdx < 0 && start < strLen; start++)
    {
        char32 c = data[start];
        for (int i = 0; i < 3; i++)
        {
            if (c == oDelimiters[i])
            {
                delIdx = i;
                break;
            }
        }
    }

    if (delIdx < 0)
    {
        return NULL;
    }

    int end, count = 1;
    for (end = start + 1; end < strLen; end++)
    {
        char32 c = data[end];

        if (c == oDelimiters[delIdx]) count++;
        else if (c == cDelimiters[delIdx]) count--;

        if (count <= 0) break;
    }

    if (count > 0)
    {
        return NULL;
    }

    return UniStr_slice(string, start, end);
}

void UnitTest_UniStr_encodeU8(void)
{
    UniStr *uniStr = UniStr_decodeU8(
        u8"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ", -1
    );
    for (int i = 0; i < uniStr->length; i++)
    {
        assert((int)uniStr->data[i] == 0xC0 + i);
    }
    UniStr_destroy(uniStr);
}

void UnitTest_UniStr_decodeAscii(void)
{
    char strAscii[] =
        "Voix ambigue d'un coeur qui, au zephyr, prefere les jattes de kiwis.";
    UniStr *uniStr = UniStr_decodeU8(
        u8"Voix ambiguë d’un cœur qui, au zéphyr, préfère les jattes de kiwis.", -1
    );
    char *buffer = UniStr_encodeAscii(uniStr);
    assert(strcmp(buffer, strAscii) == 0);
    free(buffer);
    UniStr_destroy(uniStr);
}

void UnitTest_UniStr_split(void)
{
    UniStr *string = UniStr_decodeU8(u8"Ceci;représente;un;fichier;csv", -1);
    UniStr *sep = UniStr_decodeU8(u8";", -1);
    int subCount = 0;
    UniStr **subs = UniStr_split(string, sep, &subCount);

    printf("subCount = %d\n", subCount);
    for (int i = 0; i < subCount; i++)
    {
        UniStr_print(subs[i]);
        printf("\n");
    }
    UniStr_destroyArray(subs, subCount);
    UniStr_destroy(string);
    UniStr_destroy(sep);
}

void UnitTest_UniStr_getBlock(void)
{
    UniStr *string = UniStr_decodeU8(u8"{\"id\":138616553,\"type\":\"way\",\"tags\":{\"highway\":\"residential\",\"lit\":\"yes\",\"maxspeed\":\"30\",\"name\":\"Rue de la Motte Carrée\",\"source\":\"cadastre-dgi-fr source : Direction Générale des Impôts - Cadastre;mise à jour : 2010\",\"surface\":\"asphalt\"},\"centroid\":{\"lat\":\"48.0829002\",\"lon\":\"-1.0950376\"},\"bounds\":{\"e\":\"-1.0941139\",\"n\":\"48.0835946\",\"s\":\"48.0828770\",\"w\":\"-1.0961972\"},\"nodes\":[{\"lat\":\"48.0835946\",\"lon\":\"-1.0941139\"},{\"lat\":\"48.0829792\",\"lon\":\"-1.0947970\"},{\"lat\":\"48.0829245\",\"lon\":\"-1.0949553\"},{\"lat\":\"48.0828770\",\"lon\":\"-1.0951164\"},{\"lat\":\"48.0828816\",\"lon\":\"-1.0952651\"},{\"lat\":\"48.0830161\",\"lon\":\"-1.0961972\"}]}", -1);
    UniStr *tag = UniStr_decodeU8(u8"\"nodes\"", -1);
    int index = UniStr_rfind(string, tag);
    UniStr *block = UniStr_getBlock(string, index);

    UniStr_print(block);
    printf("\n\n");

    index = 0;
    UniStr *pointStr = UniStr_getBlock(block, index);
    while (pointStr != NULL)
    {
        index += pointStr->length;
        UniStr_print(pointStr);
        printf("\n");

        int end = 0;
        double lat = UniStr_getDouble(pointStr, 7, &end);
        double lon = UniStr_getDouble(pointStr, end + 9, &end);

        printf("\t%.20lf %.20lf\n", lat, lon);

        UniStr_destroy(pointStr);
        pointStr = UniStr_getBlock(block, index);
    }
    UniStr_destroy(block);

    UniStr_destroy(string);
    UniStr_destroy(tag);
}
