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
   - [Generate password](#generate-password)
   - [Password encryption](#password-encryption)
   - [Import and export password data](#import-and-export-password-data)
   - [Reorder password entries](#reorder-password-entries)
  
## Application functionality
- User authentication (login to the application)
- Add, edit, and delete login-password pairs for specific services
- Assign passwords to user-defined groups
- Generate strong random passwords
- Store all passwords in encrypted form in a local PostgreSQL database
- Import and export password data to/from CSV, JSON, and XML files
- Reorder password entries based on user preferences

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

## Setup
You can configure and run the project in two ways: by using a development environment or via a prebuilt release package.

## Method I – Manual setup using Qt Creator
### 1. Install Qt environment
- Download and install [Qt](https://www.qt.io/download-dev)
- Make sure to install a compatible Qt version (e.g., Qt 6.x)

### 2. Download or clone the project
- Option 1: Download the ZIP file from GitHub and extract it
- Option 2: Clone the repository:
```bash
git clone https://github.com/krystianbeduch/password-manager.git
```

### 3. Open the project in Qt Creator
- Open the `CMakeLists.txt` file from the root directory
- Configure the compiler
- Qt Creator will load the entire project automatically

### 4. Set up PostgreSQL
- Download and install [PostgreSQL](https://www.postgresql.org/download/) (recommended version: 16 or higher)
- The default superuser is `postgres`, with the default password set to `root`, and the default port is `5432`
> [!NOTE]
> Password and port is set during installation

- Create a new database using the following SQL command (you can use psql or graphical client `pgAdmin`). The name of the base is arbitrary:
```sql
CREATE DATABASE password_manger;
```

### 5. Create the database configuration file
- In the `database` directory, create a file named `db_config.json`
- Define the following configuration (adjust the values if necessary):
```json
{
   "database": {
      "host": "localhost",
      "port": 5432,
      "db_name": "password_manager",
      "username": "postgres",
      "password": "root"
   }
}
```
> [!NOTE]
> Ensure that the file name and location are correct, and that the application has access to read this file at runtime.

### 6. Run the application
- If the environment and the database have been properly configured, the application should launch successfully
- Upon the first launch, you nned to create a new account
- For more details, see the [User authentication](#user-authentication) section

## Method II – Use prebuilt release
You can run the application using a precompiled release version available for [download](https://github.com/krystianbeduch/password-manager/releases/download/v1.0/password_manager.zip)
> [!NOTE]
> The following instructions are also available in the `README.txt` file included with the release.

### 1. Download the prebuilt release
- Download the latest compiled version of `Password Manager` from the Releases section
- Extract the ZIP archive to any directory on your system

### 2. Install and configure PostgreSQL
- To use the Password Manager application, you must have a properly configured local PostgreSQL database
- Download PostgreSQL from the official website:
   - https://www.postgresql.org/download/
   - https://www.enterprisedb.com/downloads/postgres-postgresql-downloads
- The minimum recommended version is PostgreSQL 16
- During the installation pay attention:
   - on the installation path - it is best to leave the default one in the `Program Files` directory
   - on the password for the postgres superuser - you should enter the password `root`
   - the port on which the database will run - leave the default `5432`

If you specify a different password or port, the script and application will not work without modifying the configuration files.

### 3. Database setup
Once PostgreSQL is installed, you need to configure the database. You have two options:

#### Option A – Automatic configuration:
- Run the included `init_database.bat` script
- After successful execution, you should see a message similar to this:
```bash
Creating database password_manager...
Creating schema public (if it doesn't exist)...
CREATE SCHEMA
Granting permissions on schema public to user postgres...
GRANT
Done!
Press any key to continue . . .
```

#### Option B – Manual configuration:
If you prefer to create the database manually, launch the `pgAdmin` client and create the database in the GUI or via SQL command:
```sql
CREATE DATABASE password_manger;
```

### 4. Fixing 'psql' is not recognized as an internal or external command error
If you selected option A, and the following error message appears when you run the script:
```bash
'psql' is not recognized as an internal or external command
```
...it means that the `psql` command-line tool (PostgreSQL client) is not found in the system's PATH variable.

To fix this, follow these steps to add PostgreSQL to your system PATH:

#### Step-by-step:
1. Locate the PostgreSQL installation directory
   - By default, it should be something like: `C:\Program Files\PostgreSQL\16`<br> (the number 16 refers to the installed version).

2. Enter the bin subdirectory
   - This directory contains the `psql.exe` and other necessary executables.

3. Copy the full path to the bin directory
   - Click the address bar at the top of the File Explorer window and copy the full directory path.

4. Open the System Environment Variables settings
   - Press the Windows key or click Start and search for: `Edit the system environment variables`

5. In the window that appears, click the `Environment Variables...` button
6. In the "System variables" section, locate and select the variable named `Path`, then click `Edit...`
7. Click `New` and paste the copied PostgreSQL bin directory path
8. Confirm by clicking `OK` in all windows to apply the changes
9. Restart the `init_database.bat` script
After the PATH has been updated, the system should recognize the psql command and the script should run without errors.

## Functional description
### User authentication
When launching the application, a login dialog appears prompting the user to enter an access password. If the user does not remember the password or is using the application for the first time, they can check the:
- [x] Create new profile<br>
Creating a new profile is equivalent to erasing all existing data. This means that if the user has forgotten their previous password, there is no way to recover the old data, and they must accept its loss.

After creating a new profile, all existing tables in the database are cleared and reinitialized with default values. These include:
- 4 default groups: Work, Personal, Banking, Email
- 3 example entries to demonstrate how the application works

The entered access password is used to generate an encryption key, which is essential for securely storing and retrieving sensitive user data. 
Additionally, to prevent unauthorized access, the login dialog will automatically reappear after 5 minutes of inactivity. 
This ensures that if the application is left unattended, access to the data remains protected.
<p align="center">
   <img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/user_authentication.png" alt="User authentication" title="User authentication">
   <img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/user_authentication_new_account_table.png" alt="User authentication - new account" title="User authentication - new account">
</p>

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

<img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/password_management_show_action.png" alt="Show action" title="Show action">

➕ Adding new password<br>
Users can add new password entries using multiple methods:
- `Ctrl + N` keyboard shortcut
- clicking the plus icon in the top toolbar
- Menu: `Management -> Add Password`<br>

This action opens a form where the user inputs password data.
<p align="center">
   <img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/password_management_adding_new_password.png" alt="Adding new password" title="Adding new password">
</p>
   
📝 Editing existing password<br>
Passwords can be edited using:
- `Ctrl + E` keyboard shortcut
- clicking the edit icon in the top toolbar
- Menu: `Management -> Edit Password`<br>

When triggered, the user is first asked to select the entry to be edited. After selecting it, the same input form as for adding a new password is shown.

<p align="center">
   <img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/password_management_select_dialog_edit.png" alt="Select dialog" title="Select dialog">
   <img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/password_management_edit_action.png" alt="Editing password" title="Editing password">
</p>

🗑️ Deleting password<br>
Passwords can be deleting using:
- `Ctrl + Del` keyboard shortcut
- click the trash icon in the toolbar
- Menu: `Management -> Delete Password`<br>

Each deletion requires confirmation to avoid data loss.
<p align="center">
   <img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/password_management_delete_action.png" alt="Deleting password" title="Deleting password">
</p>
Additionally, users can remove all password entries at once by selecting: `Management -> Delete All Passwords`<br>
This action is also protected by confirmation dialogs to prevent unintentional mass deletion.

### Group management
Groups provide a convenient way to organize password entries based on user preferences. Each password can be assigned to a specific group, making it easier to categorize and manage entries within the application.

By default, the application comes with four pre-defined groups:
- Work
- Personal
- Banking
- Email

➕ Adding group<br>
Group creation is integrated directly into the Add and Edit Password forms. When a user enters a new group name, the application checks whether the name is unique. Duplicate group names are not allowed.

✏️ Editing or 🗑️ Deleting group<br>
Users can modify or remove existing groups through dedicated dialog windows. These dialogs work similarly to the ones used for editing or deleting password entries, allowing the user to select a group from a list.

> [!WARNING]
> A group cannot be deleted if it still contains at least one password entry.

### Generate password
To enhance security, users can generate strong, random passwords directly within the application. This feature is especially useful when creating credentials for new services, ensuring that passwords are difficult to guess or brute-force. The generated password consists of 24 random characters. It includes a mix of uppercase letters, lowercase letters, digits, and special characters for increased complexity. Passwords are generated directly from the Add or Edit Password forms using a built-in button. This approach encourages good security practices by avoiding the reuse of weak or predictable passwords. Once generated, the password can be saved to the appropriate entry, or the user can generate another one with a single click.

<p align="center">
<img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/generate_passwords.png" alt="Generate password" title="Generate password">
</p>

> [!TIP]
> It’s recommended to use the password generator for all new entries to maximize account safety.

### Password encryption
All passwords stored in the application are encrypted using the __`ChaCha20-Poly1305`__ authenticated encryption algorithm, which is known for its performance and high security. The encryption process is built using the `libsodium` library and follows modern cryptographic practices.

#### 🔐 How it works:
#### 1. Salt generation
For each encryption operation, a unique salt is generated to ensure that derived keys are different even if the same password is used again:
```cpp
QByteArray salt(crypto_pwhash_SALTBYTES, 0);
randombytes_buf(salt.data(), salt.size());
```

#### 2. Key derivation
A secure encryption key is derived from the user's master password and the generated salt using a password hashing function. This ensures that each user profile has a unique encryption key, even if the same password is used across multiple profiles.

#### 3. Nonce initialization
A random nonce is created for each encryption operation to guarantee that the same plaintext encrypted multiple times will yield different ciphertexts:
```cpp
QByteArray nonceOut;
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

#### 5. 🗄️ Database storage
Encrypted data is stored in the PostgreSQL database using the `bytea` type. For each password entry, the following three binary fields are saved:
- cipherText — the encrypted password
- salt — used to derive the encryption key
- nonce — required for proper decryption

This separation ensures secure, deterministic decryption only when the correct master password is provided.

> [!WARNING]
> The encryption key is never stored — it is derived in memory from the user's master password and the stored salt each time the application is launched.

### Import and export password data
The application supports importing and exporting password data to facilitate backup, migration, or bulk entry.

#### Import passwords
Passwords can be imported via:
- Menu: `Management -> Import Password...`

This will open a file selection dialog. The supported file formats for import are:
- CSV
- JSON
- XML

Sample input files and format structure examples can be found in the `services` module:
- [`test_file.csv`](./services/test_file.csv)
- [`test_file.json`](./services/test_file.json)
- [`test_file.xml`](./services/test_file.xml)

> [!IMPORTANT]
> Before importing, make sure that all password groups referenced in the import file already exist in the application.
If a group does not exist, the import will fail. You must create any missing groups manually beforehand.

<img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/import_passwords.png" alt="Import passwords" title="Import passwords">

#### Export passwords
Passwords can be exported via:
- Menu: `Management -> Export Passwords...`

A dialog window will appear, allowing you to:
- select which passwords to export
- choose the output format(s): CSV, JSON, or XML
- specify the file name(s) for the exported data

This feature is particularly useful for creating backups or transferring data between different instances of the application.
<p align="center">
   <img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/export_passwords.png" alt="Export passwords" title="Export passwords">
</p>

### Reorder password entries
The password list displayed in the application can be sorted in two different ways:
- attribute-based sorting – ascending or descending order based on a selected column (e.g., service name, group)
- custom order – new entries are always added to the end of the list, and the user can manually rearrange the orde.

When using custom order, users can move selected entries according to their own preferences. This can be done in several ways:
- Move up by one position:
   - `Ctrl + ↑` keyboard shortcut
   - click the up arrow icon in the toolbar
   - Menu: `Table -> Move password up`
- Move down by one position:
   - `Ctrl + ↓` keyboard shortcut
   - click the down arrow icon in the toolbar
   - Menu: `Table -> Move password down`
- Move to the beginning of the list:
   - `Ctrl + Shift + ↑` keyboard shortcut
   - click the rewind icon in the toolbar
   - Menu: `Table -> Move password to the beginning`
- Move to the end of the list:
   - `Ctrl + Shift + ↓` keyboard shortcut
   - click the fast-forward icon in the toolbar
   - Menu: `Table -> Move password to the end`
   
Once a password entry has been moved, a message appears in the status bar: `Order has been changed - don't forget to save`. To save the new order use:
- `Ctrl + S` keyboard shortcut
- click the save icon in the toolbar
- Menu: `Table -> Save password position`<br>

> [!IMPORTANT]
> You cannot reorder entries while attribute-based sorting is active.<br>
> You must switch back to the `Sort by your own items` option first.

> [!WARNING]
> If you modify the order but try to switch to attribute-based sorting without saving, a dialog will appear prompting you to save the changes first.

<img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/reorder_password_entries_custom_order.png" alt="Reorder password entries - custom order" title="Reorder password entries - custom order">
<img src="https://github.com/krystianbeduch/password-manager/blob/main/readme_images/reorder_password_entries_sort_by_group_asc.png" alt="Reorder password entries - sort by group asc" title="Reorder password entries - sort by group asc">
