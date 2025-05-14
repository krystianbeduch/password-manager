CREATE TABLE IF NOT EXISTS passwords (
    id SERIAL PRIMARY KEY,
    service_name VARCHAR(255) NOT NULL,
    username VARCHAR(255) NOT NULL,
    password TEXT NOT NULL,
    group_name VARCHAR(255),
    addition_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
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
    password TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


