#!/usr/bin/bash
lxc-destroy 1
lxc-destroy 2

curl -X PUT "http://localhost:8001/player/1" -F env_id=0 -F automake=false -F data=@player.7z
echo
curl -X PUT "http://localhost:8001/player/2" -F env_id=0 -F automake=false -F data=@player.7z
echo
curl -X PUT "http://localhost:8001/game/1" -F env_id=0 -F data=@judge.zip
echo
curl -X PUT "http://localhost:8001/job/1?game_id=1&p1_id=1&p2_id=2"
echo
