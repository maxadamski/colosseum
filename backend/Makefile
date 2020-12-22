redis:
	redis-server --port 6666 &
	redis-server --port 6667 &

devel:
	uvicorn main:app --reload

serve:
	uvicorn main:app
