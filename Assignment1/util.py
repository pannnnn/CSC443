# The script is running on my own machine (Macbook pro 2016) and my own USB
# drive (Kingston 8GB)
import os
import numpy as np
import pylab as pl
import argparse
from subprocess import check_output

parser = argparse.ArgumentParser()
parser.add_argument("-m", "--medium", default='hdd', choices=["hdd", "ssd", "usb"],
        metavar="['hdd', 'ssd', 'usb']", help="set the storage medium")
args = parser.parse_args()
total_bytes = 100 * 1024 * 1024
block_sizes = [128]*20 + [1024]*20 + [4096]*20 + [8192]*20  + [65536]*20  + [131072]*20  + [524288]*20  + [1048576]*20  + [2097152]*20  + [3145728]*20 
file_name = "/Volumes/X/CSC443/data.txt" if args.medium == "usb" else "data.txt"

write_data_rates = [0] * 200
read_data_rates = [0] * 200
for i in range(200):
    # calculate write data rates
    write_result = check_output(["./create_random_file", file_name, str(total_bytes), str(block_sizes[i])])
    write_result = write_result.split()[1]
    write_data_rates[i] = int(total_bytes / (float(write_result) / 1000))
    # calculate read data rates
    read_result = check_output(["./get_histogram", file_name, str(block_sizes[i])])
    read_result = read_result.splitlines()[-1].split()[1]
    read_data_rates[i] = int(total_bytes / (float(read_result) / 1000))
    os.remove(file_name)

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
    usb_path = "/Volumes/X/CSC443/"
    pl.savefig(usb_path + 'sequential_write.png', bbox_inches='tight')
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
    usb_path = "/Volumes/UBUNTU 16_0/CSC443/"
    pl.savefig(usb_path + 'sequential_read.png', bbox_inches='tight')
# pl.show()
