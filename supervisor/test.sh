#!/usr/bin/bash
curl -X PUT "http://localhost:8000/player/1?env_id=2" -F data=@app.py
