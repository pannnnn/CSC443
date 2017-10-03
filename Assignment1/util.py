# The script is running on my own machine (Macbook pro 2016) and my own USB
# drive (Kingston 8GB)
import os
import sys
import numpy as np
import pylab as pl
import argparse
from subprocess import check_output

parser = argparse.ArgumentParser()
parser.add_argument("-m", "--medium", default='hdd', choices=["hdd", "ssd", "usb"], metavar="['hdd', 'ssd', 'usb']", help="set the storage medium")
parser.add_argument("-t", "--total_bytes", type=int, default=100, help="set the total size for the file (unit: MBytes)")
parser.add_argument("-p", "--path", default='', help="set the path to write data to (should be a valid path to your drive)")
args = parser.parse_args()
path = args.path
if path != '':
    if not os.path.exists(path) or path[-1] != '/':
        print("Please Enter Valid Path.")
        sys.exit()
total_bytes = args.total_bytes * 1024 * 1024
block_sizes = [128]*30 + [1024]*30 + [4096]*30 + [8192]*30  + [65536]*30  + [131072]*30  + [524288]*30  + [1048576]*30  + [2097152]*30  + [3145728]*30 
if args.medium == "hdd" or args.medium == "ssd":
    file_names = ["block_size_" + str(block_size) + "B" + str(index) + ".txt" for index, block_size in enumerate(block_sizes)]
elif args.medium == "usb":
    file_names = [path + "block_size_" + str(block_size) + "B" + str(index) + ".txt" for index, block_size in enumerate(block_sizes)]

write_data_rates = [0] * 300
read_data_rates = [0] * 300
for i in range(300):
    # calculate write data rates
    write_result = check_output(["./create_random_file", file_names[i], str(total_bytes), str(block_sizes[i])])
    write_result = write_result.split()[1]
    write_data_rates[i] = int(total_bytes / (float(write_result) / 1000))
    # calculate read data rates
    read_result = check_output(["./get_histogram", file_names[i], str(block_sizes[i])])
    read_result = read_result.splitlines()[-1].split()[1]
    read_data_rates[i] = int(total_bytes / (float(read_result) / 1000))
    os.remove(file_names[i])

# Write output to write_data.txt
f= open("write_data.txt","w")
f.write("Block Size(Byte)              Data_rates(Bytes/Second)\n")
for i in range(300):
     f.write(str(block_sizes[i]) + (30 - len(str(block_sizes[i]))) * ' ' + str(write_data_rates[i]) + "\n" )
f.close()
# Write output to read_data.txt
f= open("read_data.txt","w")
f.write("Block Size(Byte)              Data_rates(Bytes/Second)\n")
for i in range(300):
     f.write(str(block_sizes[i]) + (30 - len(str(block_sizes[i]))) * ' ' + str(read_data_rates[i]) + "\n" )
f.close()

# Write Plot
# use pylab to plot x and y
pl.plot(block_sizes, write_data_rates, 'ro', ms=3)
# give plot a title
pl.title('Write Data Rate Versus Block Size')
# make axis labels
pl.xlabel('Block Size (Byte)')
pl.ylabel('Write Data Rates (Byte/Second)')
# set discontinous points
pl.xscale('log')
# show the plot on the screen
if args.medium == "hdd" or args.medium == "ssd":
    pl.savefig('sequential_write.png', bbox_inches='tight')
elif args.medium == "usb":
    pl.savefig(path + 'sequential_write.png', bbox_inches='tight')
# pl.show()

pl.close()

# Read Plot
# use pylab to plot x and y
pl.plot(block_sizes, read_data_rates, 'ro', ms=3)
# give plot a title
pl.title('Read Data Rate Versus Block Size')
# make axis labels
pl.xlabel('Block Size (Byte)')
pl.ylabel('Read Data Rates (Byte/Second)')
# set discontinous points
pl.xscale('log')
# show the plot on the screen
if args.medium == "hdd" or args.medium == "ssd":
    pl.savefig('sequential_read.png', bbox_inches='tight')
elif args.medium == "usb":
    pl.savefig(path + 'sequential_read.png', bbox_inches='tight')
# pl.show()
