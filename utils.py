import os
import subprocess
import matplotlib.pyplot as plt


def compile(directory: str) -> str:
    """Compile a program called 'main' in the specified directory."""
    os.chdir(os.path.dirname(os.path.abspath(__file__)) + "/" + directory)
    try:
        subprocess.check_call(["mkdir", "build"])
    except:
        pass
    os.chdir(os.path.dirname(os.path.abspath(__file__)) + "/" + directory + "/build")
    subprocess.check_call(["cmake", ".."])
    subprocess.check_call(["cmake", "--build", ".", "--config", "Release"])
    os.chdir(os.path.dirname(os.path.abspath(__file__)) + "/" + directory + "/build/Release")
    files = os.listdir()
    if "main.exe" in files:
        program = "main.exe"
    else:
        program = "main"
    return directory + "/build/Release/" + program


def execute(executable: str) -> str:
    """Run the specified program."""
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    return subprocess.check_output([executable])


def decompose_results(output: str) -> dict:
    """Decompose the CSV-style results output by a program."""
    lines = output.splitlines()
    lines = [l.decode("utf-8") for l in lines]
    columns = []
    for _ in range(len(lines[0][:-1].split(","))):
        columns.append([])
    for line in lines:
        line = line[:-1]  # Ignore the trailing comma.
        cells = line.split(",")
        for i, cell in enumerate(cells):
            try:
                columns[i].append(float(cell))
            except:
                columns[i].append(cell)
    N = columns[0][1:]
    values = {}
    for column in columns[1:]:
        values[column[0]] = column[1:]
    return N, values


def read_lines(output: str) -> list:
    lines = output.splitlines()
    return [l.decode("utf-8").split(",") for l in lines]
