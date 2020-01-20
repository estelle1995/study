#ori_dicts
#res_dict
ori_dicts = [{"id":16, "name":"aaaa", "type":0}]
res_dict = {}
for ori_dict in ori_dicts:
    if ori_dict["type"] in res_dict.keys():
        res_dict[ori_dict["type"]].append(ori_dict)
    else:
        res_dict[ori_dict["type"]] = [ori_dict]
print (res_dict)