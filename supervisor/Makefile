.PHONY: redis devel serve

redis:
	#redis-server --port 7777 & disown

devel:
	uvicorn app:app --reload --port 8001

serve:
	uvicorn app:app --port 8001
