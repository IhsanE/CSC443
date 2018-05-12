import subprocess
import os


heap_generation_script_path = (os.path.realpath('csv2heapfile'))
select_script_path = (os.path.realpath('select'))
heapfile = "HEAPFILE.txt"
csv_num_tuples = 100
csv_file = "csvfile.txt"
num_runs = 1
page_size = [1.3] + list(range(2, 52))
if (not os.path.exists(csv_file)):
    print ("Generating csv file with {} records".format(csv_num_tuples))
    subprocess.call(["python mkcsv.py {} {}".format(csv_file, str(csv_num_tuples))], shell=True)

print ("Running select on 100 tuples with pages from 1000 -> 51000")

for p in page_size:
    runs = [[],[],[]]
    avgs = []
    p = p * 1000
    subprocess.call('rm -rf {}'.format(heapfile), shell=True)
    proc = subprocess.Popen(
        [heap_generation_script_path, csv_file, heapfile, str(p)],
        stdout=subprocess.PIPE
    )
    proc.communicate([0])
    for i in range(num_runs):
        # all records, half of the records, less than half of the records
        index = -1
        for combo in [("A", "Z"), ("A", "M"), ("A", "D")]:
            index += 1
            select_proc = subprocess.Popen(
                [select_script_path, heapfile, "0", combo[0], combo[1], str(p)],
                stdout=subprocess.PIPE
            )
            stdout = select_proc.communicate([0])
            time = stdout[0].split("TIME")[-1].strip().split(" ")[1]
            if (time != 'inf'):
                runs[index].append(float(time))

    for run in enumerate(runs):
        avg = sum(run[1])/num_runs
        avgs.append(avg)

    print("average time (ms) for page size ({}):(A,Z): {} (A,M): {} (A,D): {}".format(
        p,
        avgs[0],
        avgs[1],
        avgs[2]
    ))
