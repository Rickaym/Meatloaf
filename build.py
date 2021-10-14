import os
import json

with open("config.json", "r") as f:
    CONFIG = json.load(f)

def cexec(cmdlet):
    for cmd in cmdlet.split(";"):
        print(f"RUNNING {cmd}")
        if os.system(cmd) == 1:
            print(f"BUILD RECEEDING... FAILURE")
            break

pth = CONFIG['path']
runs = CONFIG['executable'].replace('/', '\\')
try:
    cexec(
        f"{CONFIG['compiler']} -Wall -I ./include {pth}{f'.cpp {pth}'.join(CONFIG['src'])}.cpp -o {CONFIG['executable']} \
        ;{runs} \
        ;cmd /c .\garbage.bat"
        )
except KeyboardInterrupt:
    pass