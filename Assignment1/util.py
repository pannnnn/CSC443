from subprocess import check_output
import numpy as np
import pylab as pl

total_bytes = 100 * 1024 * 1024
block_sizes = [128, 1024, 4096, 8192, 65536, 131072, 524288, 1048576, 2097152, 3145728]
file_names = ["block_size_" + str(block_size) + "B.txt" for block_size in block_sizes]
write_data_rates = [0] * 10
for i in range(10):
    result = check_output(["./create_random_file", file_names[i], str(total_bytes), str(block_sizes[i])])
    write_data_rates[i] = int(total_bytes / (float(result) / 1000))

# use pylab to plot x and y
pl.plot(block_sizes, write_data_rates, 'r')
# give plot a title
pl.title('Write Data Rate Versus Block Size')
# make axis labels
pl.xlabel('Block Size (Byte)')
pl.ylabel('Write Data Rates (Byte/Second)')
# show the plot on the screen
print write_data_rates
pl.show()
