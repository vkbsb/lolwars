#http://lkimg.zamimg.com/shared/riot/images/champions/1.png
import json
import os


data = json.load(open("champ.json"))

champdata = data["data"]

for key in champdata:
    champ = champdata[key]
    print champ["id"]
    fname = str(champ["id"]) + ".png"
    cmd = "curl http://lkimg.zamimg.com/shared/riot/images/champions/" + fname + " -o " + fname
    os.system(cmd)
