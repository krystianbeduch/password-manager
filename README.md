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
🔐
When launching the application, a login dialog appears prompting the user to enter an access password. If the user does not remember the password or is using the application for the first time, they can check the `Create new profile` checkbox. Creating a new profile is equivalent to erasing all existing data. This means that if the user has forgotten their previous password, there is no way to recover the old data, and they must accept its loss.

After creating a new profile, all existing tables in the database are cleared and reinitialized with default values. These include:
- 4 default groups: Work, Personal, Banking, Email
- 3 example entries to demonstrate how the application works

The entered access password is used to generate an encryption key, which is essential for securely storing and retrieving sensitive user data. 
Additionally, to prevent unauthorized access, the login dialog will automatically reappear after 5 minutes of inactivity. 
This ensures that if the application is left unattended, access to the data remains protected.

 
### Password management
- Add, edit, and delete login-password pairs for specific services
- 
### Group management
- Assign passwords to user-defined groups
 
### Generate passwords
Generate strong random passwords

### Password encryption
- Store all passwords in encrypted form in a local PostgreSQL database- 
  
### Reorder password
- Reorder password entries based on user preferences
  
### Import and export password data
- Import and export password data to/from CSV, JSON, and XML files



Presents all tasks in a scrollable list using a _RecyclerView_. Each task is visually styled according to the `item_task.xml` layout, ensuring a consistent and user-friendly appearance. 
Each task item displays the following information:
- Title — the main name or description of the task
- Deadline — the due date and time by which the task should be completed
- Creation date — the date and time when the task was originally added
- Priority — indicates the importance level of the task, which can be _High_, _Medium_, or _Low_
- Completion status — represented by a ✔ symbol when the task is marked as completed
- Attachment icon — displayed if one or more attachments are associated with the task, indicating additional files or resources linked to it
<p align="center">
   <img src="https://github.com/krystianbeduch/todo-list/blob/main/readme-images/list-of-tasks.jpg" alt="List of tasks" title="List of tasks" height="800" align="center">
</p>
This setup provides a clear and informative overview of all tasks, helping users to easily track, prioritize, and manage their work
