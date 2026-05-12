# Potoo Maps

A high-performance pathfinding engine written in C, developed during the 2nd year of engineering school. It uses OpenStreetMap data (GeoJSON) to build a road graph and calculates the shortest path using an optimized Dijkstra algorithm.

## Features
- **Fast GeoJSON Parsing**: Custom parser with pre-cleaning step for large datasets.
- **Graph Engine**: Efficient adjacency list representation of road networks.
- **Optimized Dijkstra**: Uses a Binary Heap for (E \log V)$ pathfinding.
- **Multi-Dataset Support**: Built-in support for multiple regions (ESIEA, Laval, Mayenne, Centre-Val de Loire, etc.).
- **Cross-Platform**: Compiles and runs on Linux and Windows.
- **Visual Output**: Generates standard GeoJSON output compatible with [geojson.io](https://geojson.io).

## Project Structure
- `src/`: Core logic and data structures.
- `include/`: Header files.
- `Assets/`: Raw GeoJSON and processed data.
- `external/`: External libraries (cJSON).

## Installation & Build

### Requirements
- CMake (3.10+)
- A C compiler (GCC, Clang, or MSVC)

### Build Steps
```bash
mkdir build
cd build
cmake ..
make # or build with Visual Studio on Windows
```

## Usage
The program accepts arguments for the dataset and coordinates:

```bash
./PotooMaps <dataset> <start_lat> <start_lon> <end_lat> <end_lon>
```

**Example (Tours to Orléans):**
```bash
./PotooMaps centre 47.394144 0.68484 47.902964 1.903864
```

**Available datasets:** `esiea`, `laval`, `mayenne`, `pdll`, `centre`.

### Custom Datasets
You can also use your own GeoJSON file (must be located in `Assets/`):
```bash
./PotooMaps custom Assets/my_city.geojson 48.1 -0.7 48.2 -0.6
```

## Output
The program outputs the total distance in kilometers and a GeoJSON `FeatureCollection`. Copy the JSON block and paste it into [geojson.io](https://geojson.io) to visualize the route.

## License
MIT License
