.PHONY: redis devel serve

redis:
	#redis-server --port 7777 & disown

devel:
	uvicorn app:app --reload

serve:
	uvicorn app:app
