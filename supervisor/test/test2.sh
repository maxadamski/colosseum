#!/usr/bin/bash
lxc-destroy 1
lxc-destroy 2

curl -X PUT "http://localhost:8000/player/1?env_id=2&automake=false" -F data=@player.7z
curl -X PUT "http://localhost:8000/player/2?env_id=2&automake=false" -F data=@player.7z
#curl -X PUT "http://localhost:8000/game/1?env_id=2" -F data=@judge.7z
curl -X PUT "http://localhost:8000/job/1?game_id=1&p1_id=1&p2_id=2"
echo
