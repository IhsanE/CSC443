import subprocess
import os


script_path = (os.path.realpath('csv2heapfile'))
heapfile = "HEAPFILE.txt"
csv_num_tuples = 35000
csv_file = "csvfile.txt"
num_runs = 1
page_size = [1.3, 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51]
if (not os.path.exists(csv_file)):
    print ("Generating csv file with {} records".format(csv_num_tuples))
    subprocess.call(["python mkcsv.py {} {}".format(csv_file, str(csv_num_tuples))], shell=True)

print ("Running csv2heapfile on 50000 tuples with pages from 1000 -> 51000")

subprocess.call('rm -rf {}'.format(heapfile), shell=True)
for p in page_size:
    runs = []
    p = p * 1000
    for i in range(num_runs):
        proc = subprocess.Popen(
            [script_path, csv_file, heapfile, str(p)],
            stdout=subprocess.PIPE
        )
        stdout = proc.communicate([0])
        time = stdout[0].split(":")[-1].split(" ")[1].strip()
        if (time != 'inf'):
            runs.append(float(time))
        subprocess.call('rm -rf {}'.format(heapfile), shell=True)
    avg = sum(runs)/num_runs
    print("average record/ms for page size ({}) : {}".format(
        p,
        avg
    ))
