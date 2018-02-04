import subprocess
import os


crf_path = (os.path.realpath('write_fixed_len_pages'))
out_file = "out.txt"
csv_num_tuples = 50000
csv_file = "csvfile.txt"
num_runs = 100
page_size = [1000, 5000, 10000, 50000, 100000, 500000, 1000000, 5000000, 10000000]
if (not os.path.exists(csv_file)):
    print ("Generating csv file with {} records".format(csv_num_tuples))
    subprocess.call(["python mkcsv.py {} {}".format(csv_file, str(csv_num_tuples))], shell=True)

print ("Running create_random_file on 10 diff block sizes between 100B and 3MB")
for p in page_size:
    runs = []
    for i in range(num_runs):
        proc = subprocess.Popen(
            [crf_path, csv_file, out_file, str(p)],
            stdout=subprocess.PIPE
        )
        stdout = proc.communicate([0])
        time = stdout[0].split(":")[-1].split(" ")[1]
        if (time != float('inf')):
            runs.append(float(time))

    avg = sum(runs)/num_runs
    print("average for page size ({}) : {} bps".format(
        p,
        avg
    ))
