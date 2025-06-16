CREATE TABLE IF NOT EXISTS public.groups (
    id SERIAL PRIMARY KEY,
    group_name VARCHAR(255) NOT NULL
);

CREATE TABLE IF NOT EXISTS public.passwords (
    id SERIAL PRIMARY KEY,
    service_name VARCHAR(255) NOT NULL,
    username VARCHAR(255) NOT NULL,
    group_id INTEGER REFERENCES groups(id) ON DELETE CASCADE,
    addition_date TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    position INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS public.encrypted_passwords (
    cipher_id SERIAL PRIMARY KEY,
    password_id INTEGER REFERENCES passwords(id) ON DELETE CASCADE,
    encrypted_password BYTEA NOT NULL,
    nonce BYTEA NOT NULL,
    salt BYTEA NOT NULL
);

CREATE TABLE IF NOT EXISTS public.login_data (
    id SERIAL PRIMARY KEY,
    encrypted_main_password BYTEA NOT NULL,
    salt BYTEA NOT NULL,
    nonce BYTEA NOT NULL,
    addition_date TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);