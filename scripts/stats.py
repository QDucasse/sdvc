import os
import csv
import glob
import subprocess

def isOK(filename):
    error_exp = ["blocks", "driving_phils", "mcs", "loyd", "synapse", "telephony", "brp2", "prop"]
    for word in error_exp:
        if word in filename:
            return False
    return True

if __name__ == "__main__":
    res = {}
    sdve_files = glob.glob("../sdve-beem-benchmark/**/*.sdve", recursive=True)

    # blocks driving_phils mcs loyd synapse prop
    sdve_files = [sdve_file for sdve_file in sdve_files if isOK(sdve_file)]

    for sdve_file in sdve_files:
        sdve_file_name   = sdve_file.split("/")[3][:-5]
        print(sdve_file_name)
        sdve_output_name = "bin/" + sdve_file_name + ".out"
        output = subprocess.check_output(["./sdvc", "-v", "-c", sdve_file,"-o", sdve_output_name])
        instruction_number = int(output.decode().split(" ")[-1].strip())
        binary_size = os.stat(sdve_output_name).st_size
        res[sdve_file_name] = (instruction_number, binary_size)

    with open('../sdve-beem-benchmark/stats.csv', 'r') as read_obj, \
            open('../sdve-beem-benchmark/stats_1.csv', 'w', newline='') as write_obj:
        # Create a csv.reader object from the input file object
        csv_reader = csv.reader(read_obj)
        # Create a csv.writer object from the output file object
        csv_writer = csv.writer(write_obj)
        # Read each row of the input csv file as list
        for row in csv_reader:
            # Append the binary size and
            if row[0] in res:
                row.append(res[row[0]][0])
                row.append(res[row[0]][1])
                # Add the updated row / list to the output file
                csv_writer.writerow(row)
            else:
                csv_writer.writerow(row)
