CREATE TABLE IF NOT EXISTS passwords (
    id SERIAL PRIMARY KEY,
    service_name VARCHAR(255) NOT NULL,
    username VARCHAR(255) NOT NULL,
    group_name VARCHAR(255),
    addition_date TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    position INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS encrypted_passwords (
    cipher_id SERIAL PRIMARY KEY,
    password_id INTEGER REFERENCES passwords(id) ON DELETE CASCADE,
    encrypted_password BYTEA NOT NULL,
    nonce BYTEA NOT NULL,
    salt BYTEA NOT NULL
);

CREATE TABLE IF NOT EXISTS login_data (
    id SERIAL PRIMARY KEY,
    encrypted_main_password BYTEA NOT NULL,
    salt BYTEA NOT NULL,
    nonce BYTEA NOT NULL
);

-- INSERT INTO passwords (service_name, username, password, group_name)
-- VALUES
-- ('Google', 'user1', 'password123', 'social media'),
-- ('Facebook', 'user2', 'mypassword456', 'social media'),
-- ('CorporateEmail', 'user3', 'securePassword789', 'work');
