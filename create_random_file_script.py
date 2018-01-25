import subprocess
import os


crf_path = (os.path.realpath('create_random_file'))
dummy_file = "ihsan_file"
file_size = 8000000
num_runs = 100
block_sizes = [100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 3000000]
for bs in block_sizes:
    runs = []
    for i in range(num_runs):
        proc = subprocess.Popen(
            [crf_path, dummy_file, str(file_size), str(bs)],
            stdout=subprocess.PIPE
        )
        stdout = proc.communicate([0])
        bps = float(stdout[0].split(":")[1].split(" ")[1])
        if (bps != float('inf')):
            runs.append(bps)

    avg = sum(runs)/num_runs
    print("average for block size ({}) : {} bps".format(
        bs,
        avg
    ))
