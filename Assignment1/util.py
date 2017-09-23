from string import ascii_uppercase
res = '['
for c in ascii_uppercase:
    res += "'" + c + "', "
res = res[:-2]
res += ']'
print(res)
