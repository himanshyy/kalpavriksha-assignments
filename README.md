# Assignment 2 – CRUD Operations in C

## Overview

This assignment implements a basic CRUD (Create, Read, Update, Delete) system in C using file handling.

The program stores user data in a text file named `users.txt`.

## User Information

Each user contains:

- ID
- Name
- Age

The ID of each user must be unique.

## Features

- **Create:** Add a new user.
- **Read:** Display all stored users.
- **Update:** Update an existing user's name and age using the user ID.
- **Delete:** Delete a user using the user ID.
- **Persistent Storage:** Store user data in `users.txt`.

## How to Run

### 1. Compile the program

Open a terminal in the `Assignment-2` folder and run:

```bash
gcc assignment2.c -o assignment2
