CREATE TABLE IF NOT EXISTS passwords (
    id SERIAL PRIMARY KEY,
    service_name VARCHAR(255) NOT NULL,
    username VARCHAR(255) NOT NULL,
    group_name VARCHAR(255),
    addition_date TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    position INTEGER DEFAULT 0
);
-- INSERT INTO passwords (service_name, username, password, group_name)
-- VALUES
-- ('Google', 'user1', 'password123', 'social media'),
-- ('Facebook', 'user2', 'mypassword456', 'social media'),
-- ('CorporateEmail', 'user3', 'securePassword789', 'work');
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(255) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE,
    password_hash BYTEA NOT NULL,
    password_salt BYTEA NOT NULL,
    password_iterations INTEGER NOT NULL DEFAULT 100000,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS encrypted_passwords (
    cipher_id SERIAL PRIMARY KEY,
    password_id INTEGER REFERENCES passwords(id) ON DELETE CASCADE,
    encrypted_password BYTEA NOT NULL,
    nonce BYTEA NOT NULL,
    salt BYTEA NOT NULL
);
