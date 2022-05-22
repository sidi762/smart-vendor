# -*- coding: utf-8 -*-
# Copyright (c) 2022 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from ast import parse
import json
import sys
import os
import time
import argparse
import re

def EnterCmd(mycmd, waittime = 0, printresult = 1):
    if mycmd == "":
        return
    with os.popen(mycmd) as p:
        result = p.readlines()
    if printresult == 1:
        print(result)
        sys.stdout.flush()
    if waittime != 0:
        time.sleep(waittime)
    return result

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='manual to this script')
    parser.add_argument('--config', type=str, default = './app_capture_screen_test_config.json')
    parser.add_argument('--anwser_path', type=str, default = 'screenshot\\RK\\')
    parser.add_argument('--save_path', type=str, default = './report/screenshot_1/')
    args = parser.parse_args()

    with open(args.config) as f:
        all_app = json.load(f)

    cmp_status = 0
    global_pos = all_app[0]
    return_cmd = "hdc_std shell input -M -m {} {} -c 0".format(global_pos['return-x-y'][0], global_pos['return-x-y'][1])
    recent_cmd = "hdc_std shell input -M -m {} {} -c 0".format(global_pos['recent-x-y'][0], global_pos['recent-x-y'][1])
    home_cmd = "hdc_std shell input -M -m {} {} -c 0".format(global_pos['home-x-y'][0], global_pos['home-x-y'][1])
    recent_del_cmd = "hdc_std shell input -M -m {} {} -c 0".format(global_pos['recent_del-x-y'][0], global_pos['recent_del-x-y'][1])
    os.system("hdc_std kill")
    os.system("hdc_std start")
    EnterCmd("hdc_std list targets", 1)
    EnterCmd("hdc_std list targets", 1)
    EnterCmd("hdc_std list targets", 1)
    EnterCmd("hdc_std shell rm -rf /data/screen_test/train_set")
    EnterCmd("hdc_std shell mkdir -p /data/screen_test/train_set")
    EnterCmd("hdc_std file send {} {}".format(os.path.join(os.path.dirname(args.config), "printscreen"), "/data/screen_test/"))
    EnterCmd("hdc_std shell chmod 777 /data/screen_test/printscreen")
    for single_app in all_app[1:]:
        print(single_app['entry'])
        print(single_app['all_actions'])
        sys.stdout.flush()
        call_app_cmd = "hdc_std shell " + single_app['entry']
        send_file_cmd = "hdc_std file send {} {}"
        capture_screen_cmd = "hdc_std shell /data/screen_test/printscreen -f /data/screen_test/{}"
        recv_file_cmd = "hdc_std file recv /data/screen_test/{} {}"
        cmp_cmd = "hdc_std shell \"cmp -l /data/screen_test/{} /data/screen_test/train_set/{} | wc -l\""
        if single_app['entry'] != "":
            EnterCmd(call_app_cmd, 3)
        for single_action in single_app['all_actions']:
            #shot_cmd is stable, different to other cmd,so handle it specialy 
            if type(single_action[1]) == str and single_action[1] == 'shot_cmd':
                if len(single_action) == 3:
                    pic_name = single_action[2] + ".png"
                    raw_pic_name = single_action[2] + ".pngraw"
                else:
                    pic_name = single_app['app_name'] + ".png"
                    raw_pic_name = single_app['app_name'] + ".pngraw"
                EnterCmd(capture_screen_cmd.format(pic_name), 1)
                EnterCmd(recv_file_cmd.format(pic_name, args.save_path), 1)
                EnterCmd(recv_file_cmd.format(raw_pic_name, args.save_path), 1)
                next_cmd = ""
            #cmp_cmd-level is stable, different to other cmd,so handle it specialy 
            elif type(single_action[1]) == str and single_action[1] == 'cmp_cmd-level':
                next_cmd = ""
                print(send_file_cmd.format(os.path.join(args.anwser_path, raw_pic_name), "/data/screen_test/train_set"))
                sys.stdout.flush()
                EnterCmd(send_file_cmd.format(os.path.join(args.anwser_path, raw_pic_name), "/data/screen_test/train_set"))
                new_cmp_cmd = cmp_cmd.format(raw_pic_name, raw_pic_name)
                if len(single_action) == 3:
                    tolerance = single_action[2]
                else:
                    tolerance = global_pos['cmp_cmd-level'][1]
                p = EnterCmd(new_cmp_cmd, single_action[0])
                num = re.findall(r'[-+]?\d+', p[0])
                if type(num) == list and len(num) > 0 and int(num[0]) < tolerance:
                    print("{} screenshot check is ok!\n\n".format(raw_pic_name))
                else:
                    print("ERROR:{} screenshot check is abnarmal!\n\n".format(raw_pic_name))
                    cmp_status = cmp_status + 1
                sys.stdout.flush()
            #other cmd handle
            elif type(single_action[1]) == str:
                if single_action[1] not in single_app.keys():
                    target_ = global_pos[single_action[1]]
                else:
                    target_ = single_app[single_action[1]]
                #this cmd is real cmd,and have a except answer
                if type(target_[0]) == str:
                    next_cmd = ""
                    p = EnterCmd(target_[0], single_action[0])
                    result = "".join(p)
                    if len(target_) > 1:
                        findsome = result.find(target_[1], 0, len(result))
                        if findsome != -1:
                            print("\"{}\" execut result success!\n\n".format(target_[0]))
                        else:
                            print("ERROR:\"{}\" execut result failed!\n\n".format(target_[0]))
                            cmp_status = cmp_status + 1
                        sys.stdout.flush()
                #this cmd only is a name of x,y postion, to get x,y an click it
                else:
                    next_cmd = "hdc_std shell input -M -m {} {} -c 0".format(target_[0], target_[1])
            #input x,y postion, to click it
            else:
                next_cmd = "hdc_std shell input -M -m {} {} -c 0".format(single_action[1], single_action[2])
            EnterCmd(next_cmd, single_action[0])

    if cmp_status != 0:
        print("ERROR:screenshot check is abnarmal {}".format(cmp_status))
        print("End of check, test failed!")
    else:
        print("screenshot check is ok {}".format(cmp_status))
        print("End of check, test succeeded!")
    sys.stdout.flush()
    sys.exit(cmp_status)