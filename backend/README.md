# AI Colosseum Backend App

### 1. Install dependencies:

* Anaconda
* PostgreSQL

### 2. Create and configure the database and user:

Using the default 'postgres' database and default user:

```mysql
CREATE USER colosseumdb_admin WITH PASSWORD 'pwd';
CREATE DATABASE colosseumdb;
```

Switch to our database as our new user:

```
\c colosseumdb colosseumdb_admin
```

Create the schema using the 'make_schema.sql' file:

```
\include database/make_schema.sql
```

### 3. Create conda environment with necessary requirements:

```
conda env create -f environment.yml
conda activate backend
```

### 4. Start the API:

Create redis instances:

```
make redis
```

Run the api with uvicorn:

```
make serve
```

Test the API using the generated swagger docs: http://127.0.0.1:8000/docs# 