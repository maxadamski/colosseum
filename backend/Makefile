redis:
	redis-server --port 6666 & disown
	redis-server --port 6667 & disown

devel:
	uvicorn main:app --reload

serve:
	uvicorn main:app