# Password manager
The project was developed as part of my IT studies during a multi-platform application design class.

Password manager is a desktop application for securely managing a user's password collection, written in C++ using the Qt library. 
The project offers an intuitive graphical interface and a number of features, such as encrypting data, organizing passwords into groups, generating random passwords, 
and importing and exporting data in various formats.
All passwords are encrypted and stored in a local PostgreSQL database to ensure data confidentiality. 
Additionally, the application requires user authentication before access is granted, providing an extra layer of security.
The entire project is modular in structure and built using the CMake build system.

### Contents
1. [Application functionality](#application-functionality)
2. [Technology](#technology)
3. [Project structure](#project-structure)
4. [Database schema](#database-schema)
5. [Setup](#setup)
6. [Functional description](#functional-description)
   - [User authentication](#user-authentication)
   - [Password management](#password-management)
   - [Group management](#group-management)
   - [Generate passwords](#generate-passwords)
   - [Password encryption](#password-encryption)
   - [Reorder password](#reorder-password)
   - [Import and export password data](#import-and-export-password-data)
  
## Application functionality
- User authentication (login to the application)
- Add, edit, and delete login-password pairs for specific services
- Assign passwords to user-defined groups
- Generate strong random passwords
- Store all passwords in encrypted form in a local PostgreSQL database
- Reorder password entries based on user preferences
- Import and export password data to/from CSV, JSON, and XML files

## Technology
- Qt 6.9.0
- C++20
  - __libsodium__ - cryptographic library used for secure encryption
  - __ChaCha20-Poly1305__ - authenticated encryption algorithm for protecting stored passwords
- PostgreSQL 16
- CMake

## Project structure
```bash
│   app_icon.rc
│   CMakeLists.txt
│   main.cpp
│   mainwindow.cpp
│   mainwindow.h
│   mainwindow.ui
│   resources.qrc
│
├───database
│       CMakeLists.txt
│       databasemanager.cpp
│       databasemanager.h
│       db_config.json
│       db_query.sql
│
├───dialogs
│       CMakeLists.txt
│       exportpassworddialog.cpp
│       exportpassworddialog.h
│       exportpassworddialog.ui
│       logindialog.cpp
│       logindialog.h
│       logindialog.ui
│       passwordformdialog.cpp
│       passwordformdialog.h
│       passwordformdialog.ui
│       selectpassworddialog.cpp
│       selectpassworddialog.h
│       selectpassworddialog.ui
│
├───encryption
│       CMakeLists.txt
│       cryptodata.h
│       encryptionutils.cpp
│       encryptionutils.h
│
├───icons
│       app_icon.ico
│       app_icon.png
│
├───models
│       CMakeLists.txt
│       group.cpp
│       group.h
│       passwordmanager.cpp
│       passwordmanager.h
│       passwordmode.h
│
└───services
        CMakeLists.txt
        fileservice.cpp
        fileservice.h
        test_file.csv
        test_file.json
        test_file.xml
```
- Root Directory - Main Application Entry & Core Files:
   - `main.cpp` - application entry point
   - `mainwindow` - main window class managing the central UI and logic
   - `app_icon.rc` - resource script for Windows-specific application icon integration
   - `resources.qrc` - Qt resource file that embeds images and icons into the application binary
- `database` - handles all operations related to database management and queries:
   - `databasemanager` - manages PostgreSQL database connections, execution of SQL queries, and result handling
   - `db_config.json` - configuration file for database connection
   - `db_query.sql` - SQL file containing schema definitions and prepared queries
- `dialogs` - contains all the user interface dialogs used throughout the application:
   - `exportpassworddialog` - dialog for selecting exported passwords to external files
   - `logindialog` - dialog for user authentication when starting the application (or creating a new profile)
   - `passwordformdialog` - form for adding or editing passwords, it is also possible to manage groups
   - `selectdialog` - interface for selecting a password/group to edit/delete it
- `encryption` - provides all functionality related to encrypting and decrypting sensitive data:
   - `encryptionutils` - utility class for performing cryptographic operations
   - `cryptodata.h` - structure for handling encrypted data
- `models` - contains application-specific data classes:
   - `group` - groups for organizing passwords
   - `passwordmanager` - core class handling password storage
   - `passwordmode.h` - enum for display modes
- `services`: 
   - `fileservice` - manages file operations such as reading configuration files, importing and exporting passwords to CSV, JSON and XML formats
   - [`test_file.csv`](./services/test_file.csv) - contains sample data and schema for the CSV file
   - [`test_file.json`](./services/test_file.json) - contains sample data and schema for the JSON file
   - [`test_file.xml`](./services/test_file.xml) - contains sample data and schema for the XML file

## Database schema
<img src="https://github.com/krystianbeduch/password-manager/blob/main/database/db_schema.png" alt="Database schema" title="Database schema" height="350">


## Functional description
### User authentication
When launching the application, a login dialog appears prompting the user to enter an access password. If the user does not remember the password or is using the application for the first time, they can check the `Create new profile` checkbox. Creating a new profile is equivalent to erasing all existing data. This means that if the user has forgotten their previous password, there is no way to recover the old data, and they must accept its loss.

After creating a new profile, all existing tables in the database are cleared and reinitialized with default values. These include:
- 4 default groups: Work, Personal, Banking, Email
- 3 example entries to demonstrate how the application works

The entered access password is used to generate an encryption key, which is essential for securely storing and retrieving sensitive user data. 
Additionally, to prevent unauthorized access, the login dialog will automatically reappear after 5 minutes of inactivity. 
This ensures that if the application is left unattended, access to the data remains protected.

### Password management
The main window of the application displays a table containing the user's saved password entries. Each row in the table includes the following columns:
- ID - identificator of the entry
- Service name – the name of the website or service
- Username – the associated login
- Password – shown as bullet dots for security __•__
- Addition date – when the entry was created
- Actions – quick access to: `Show`, `Edit`, or `Delete` the entry

🔍 Actions on a single entry:
- Show – reveals the actual password in plain text
- Edit – opens an edit form where the user can modify the service name, username, password, or assigned group
- Delete – deletes the entry after a confirmation dialog to prevent accidental loss

➕ Adding New Passwords<br>
Users can add new password entries using multiple methods:
- `Ctrl + N` keyboard shortcut
- clicking the plus icon in the top toolbar
- Menu: `Management -> Add Password`<br>
This action opens a form where the user inputs password data.

📝 Editing Existing Passwords<br>
Passwords can be edited using:
- `Ctrl + E` keyboard shortcut
- clicking the edit icon in the top toolbar
- Menu: `Management -> Edit Password`<br>
When triggered, the user is first asked to select the entry to be edited. After selecting it, the same input form as for adding a new password is shown.

🗑️ Deleting Passwords<br>
Passwords can be deleting using:
- `Ctrl + Del` keyboard shortcut
- click the trash icon in the toolbar
- Menu: `Management -> Delete Password`<br>
Each deletion requires confirmation to avoid data loss.

Additionally, users can remove all password entries at once by selecting: `Management -> Delete All Passwords`<br>
This action is also protected by confirmation dialogs to prevent unintentional mass deletion.

### Group management
Groups provide a convenient way to organize password entries based on user preferences. Each password can be assigned to a specific group, making it easier to categorize and manage entries within the application.

By default, the application comes with four pre-defined groups:
- Work
- Personal
- Banking
- Email

➕ Adding Groups
Group creation is integrated directly into the Add and Edit Password forms. When a user enters a new group name, the application checks whether the name is unique. Duplicate group names are not allowed.

✏️ Editing or 🗑️ Deleting Groups
Users can modify or remove existing groups through dedicated dialog windows. These dialogs work similarly to the ones used for editing or deleting password entries, allowing the user to select a group from a list.

> [!WARNING]
> A group cannot be deleted if it still contains at least one password entry.

### Generate passwords
To enhance security, users can generate strong, random passwords directly within the application. This feature is especially useful when creating credentials for new services, ensuring that passwords are difficult to guess or brute-force. The generated password consists of 24 random characters. It includes a mix of uppercase letters, lowercase letters, digits, and special characters for increased complexity. Passwords are generated directly from the Add or Edit Password forms using a built-in button. This approach encourages good security practices by avoiding the reuse of weak or predictable passwords. Once generated, the password can be saved to the appropriate entry, or the user can generate another one with a single click.

> [!TIP]
> It’s recommended to use the password generator for all new entries to maximize account safety.

### Password encryption
All passwords stored in the application are encrypted using the __`ChaCha20-Poly1305`__ authenticated encryption algorithm, which is known for its performance and high security. The encryption process is built using the libsodium `library` and follows modern cryptographic practices.

#### 🔐 How it works:
#### 1. Salt Generation
For each encryption operation, a unique salt is generated to ensure that derived keys are different even if the same password is used again:
```cpp
QByteArray salt(crypto_pwhash_SALTBYTES, 0);
randombytes_buf(salt.data(), salt.size());
```

#### 2. Key Derivation
A secure encryption key is derived from the user's master password and the generated salt using a password hashing function. This ensures that each user profile has a unique encryption key, even if the same password is used across multiple profiles.

#### 3. Nonce Initialization
A random nonce is created for each encryption operation to guarantee that the same plaintext encrypted multiple times will yield different ciphertexts:
```cpp
nonceOut.resize(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
randombytes_buf(nonceOut.data(), nonceOut.size());
```

#### 4. Encryption with libsodium
The final step uses `crypto_aead_xchacha20poly1305_ietf_encrypt` to perform authenticated encryption:
```cpp
crypto_aead_xchacha20poly1305_ietf_encrypt(
   reinterpret_cast<unsigned char *>(cipherText.data()), &cipherLen,
   reinterpret_cast<const unsigned char *>(plainText.data()), plainText.size(),
   nullptr, 0,
   nullptr,
   reinterpret_cast<const unsigned char *>(nonceOut.constData()),
   reinterpret_cast<const unsigned char *>(m_key.constData())
);
```

The result is a ciphertext that includes authentication tags to verify integrity. The `nonce` and `salt` must be stored alongside the ciphertext to correctly decrypt it later.

#### 5. 🗄️ Database Storage
Encrypted data is stored in the PostgreSQL database using the `bytea` type. For each password entry, the following three binary fields are saved:
- cipherText — the encrypted password
- salt — used to derive the encryption key
- nonce — required for proper decryption

This separation ensures secure, deterministic decryption only when the correct master password is provided.

> [!WARNING]
> The encryption key is never stored — it is derived in memory from the user's master password and the stored salt each time the application is launched.

### Reorder password
- Reorder password entries based on user preferences
  
### Import and export password data
- Import and export password data to/from CSV, JSON, and XML files
<p align="center">
  <!-- <img src="https://github.com/krystianbeduch/todo-list/blob/main/readme-images/list-of-tasks.jpg" alt="List of tasks" title="List of tasks" height="800" align="center"> -->
</p>
<!-- This setup provides a clear and informative overview of all tasks, helping users to easily track, prioritize, and manage their work -->
