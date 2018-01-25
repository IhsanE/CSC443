import subprocess
import os

crf_path = (os.path.realpath('get_histogram'))
dummy_file = "ihsan_file"
num_runs = 100
block_sizes = [100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 3000000]
block_avgs = []
print ("Running get_histogram on 10 diff block sizes between 100B and 3MB")
for bs in block_sizes:
    runs = []
    for i in range(num_runs):
        proc = subprocess.Popen(
            [crf_path, dummy_file, str(bs)],
            stdout=subprocess.PIPE
        )
        stdout = proc.communicate([0])
        bps = float(stdout[0].split("DATA RATE")[1].split(" ")[1])
        if (float('inf') != bps):
            runs.append(bps)

    avg = sum(runs)/len(runs)
    block_avgs.append(avg)
    print("average for block size ({}) : {} bps".format(
        bs,
        avg
    ))
    print(runs)
