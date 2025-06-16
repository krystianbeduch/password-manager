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
- Qt 6.9
- C++20
  - __libsodium__ - cryptographic library used for secure encryption
  - __ChaCha20-Poly1305__ — authenticated encryption algorithm for protecting stored passwords
- PostgreSQL 16
- CMake

## Project structure
```bash
│   app_icon.rc
│   CMakeLists.txt
│   CMakeLists.txt.user
│   config.json
│   main.cpp
│   mainwindow.cpp
│   mainwindow.h
│   mainwindow.ui
│   password_manager_pl_PL.ts
│   resources.qrc
│
├───database
│       CMakeLists.txt
│       config.json
│       databasemanager.cpp
│       databasemanager.h
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
```

## Database schema
