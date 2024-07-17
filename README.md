![CI](https://github.com/H-Shen/WordAdvisorLib/workflows/WordAdvisorLib_Build_And_Test/badge.svg)

## WordAdvisor

WordAdvisorLib is a C++ library that provides functionality to find the closest word match for a given input word using 
Levenshtein distance. It supports multi-threaded word matching to optimize performance. 
This project also demonstrates how to use `pybind11` to wrap C++ APIs for ease of use in Python projects.

### Features

- Use C++ 17 Standard.
- Calculate the Levenshtein distance between two words.
- Find the best match for a given word from a dictionary.
- Multithreaded word matching for improved performance.
- Python bindings to access the functionalities in Python.

### Requirements

- CMake (>=3.22)
- conda (>=24.5.0)
- make (>=3.81)

### Building the Project

1. Clone the repository:

    ```sh
    git clone https://github.com/H-Shen/WordAdvisorLib.git
    cd WordAdvisorLib
    ```

2. Activate the conda environment:

    ```sh
    conda remove -n word_advisor_lib_env_01 --all -y
    conda env create --file environment.yml
    conda activate word_advisor_lib_env_01
    ```

3. Create a build directory, navigate to it and configure the project with CMake:

    ```sh
    rm -rf build
    mkdir -p build
    cd build
    cmake ..
    ```

4. Build the project:

    ```sh
    make
    ```

5. Running Tests

   The project includes tests that can be run after building:
   
   ```sh
   ./output
   ```

6. Call methods from Python 3.11

   ```shell
   cp -vr ../test.py .
   python3.11 ./test.py
   ```
