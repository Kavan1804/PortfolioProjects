from tkinter import *
from tkinter import messagebox
import random
import pyperclip
import json

# ---------------------------- PASSWORD GENERATOR ------------------------------- #


def generate_password():
    letters = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
    numbers = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
    symbols = ['!', '#', '$', '%', '&', '(', ')', '*', '+']

    nr_letters = random.randint(8, 10)
    nr_symbols = random.randint(2, 4)
    nr_numbers = random.randint(2, 4)

    password_letters = [random.choice(letters) for _ in range(nr_letters)]
    password_symbols = [random.choice(symbols) for _ in range(nr_symbols)]
    password_numbers = [random.choice(numbers) for _ in range(nr_numbers)]

    password_list = password_letters + password_symbols + password_numbers

    random.shuffle(password_list)
    passwords = "".join(password_list)
    password_entry.insert(0, passwords)
    pyperclip.copy(passwords)

# ---------------------------- SAVE PASSWORD ------------------------------- #


def save_to_file():
    new_data = {
        website_entry.get(): {
            "Email": email_entry.get(),
            "Password": password_entry.get()
        }
    }
    if len(website_entry.get()) == 0 or len(password_entry.get()) == 0:
        messagebox.showwarning("Warning", "Dont leave the fields empty!")
    else:
        yesno = messagebox.askyesno(title="Confirmation", message=f"These are the details entered:\n Email: {email_entry.get()}"
                                                                  f"\n Website: {website_entry.get()}\n "
                                                                  f"Password: {password_entry.get()}.\n Are you sure? ")
        if yesno:
            try:
                with open("passwords.json", "r") as password_file:
                    data = json.load(password_file)
            except FileNotFoundError:
                with open("passwords.json", "w") as password_file:
                    json.dump(new_data, password_file, indent=4)
            else:
                data.update(new_data)
                with open("passwords.json", "w") as password_file:
                    json.dump(data, password_file, indent=4)
            finally:
                website_entry.delete(0, END)
                password_entry.delete(0, END)


def find_password():
    websites = website_entry.get()
    try:
        with open("passwords.json") as password_file:
            data = json.load(password_file)
    except FileNotFoundError:
        messagebox.showinfo(title="Error", message="No file found")
    else:
        if websites in data:
            emails = data[websites]["Email"]
            passwords = data[websites]["Password"]
            messagebox.showinfo(title=websites, message=f"Email: {emails}\n Password: {passwords}")
        else:
            messagebox.showinfo(title="Error", message="No details for this website")


# ---------------------------- UI SETUP ------------------------------- #


window = Tk()
window.title("Password Manager")
window.config(padx=20, pady=20)

canvas = Canvas(width=200, height=200)
logo = PhotoImage(file="logo.png")
canvas.create_image(100, 100, image=logo)
canvas.grid(column=1, row=1)

website = Label(text="Website:", font=("Times New Roman", 10, "bold"))
website_entry = Entry(width=20)
website_entry.focus()

email = Label(text="Email/Username:", font=("Times New Roman", 10, "bold"))
email_entry = Entry(width=35)
email_entry.insert(END, string="kavansiddesh@gmail.com")

password = Label(text="Password:", font=("Times New Roman", 10, "bold"))
password_entry = Entry(width=20)

generate = Button(text="Generate Password", width=11, command=generate_password)
add = Button(text="Add", width=32, command=save_to_file)
search = Button(text="Search", width=11, command=find_password)

search.grid(column=2, row=2)
generate.grid(column=2, row=4)
password_entry.grid(column=1, row=4)
email_entry.grid(column=1, row=3, columnspan=2)
website_entry.grid(column=1, row=2)
website.grid(column=0, row=2)
email.grid(column=0, row=3)
password.grid(column=0, row=4)
add.grid(column=1, row=5, columnspan=2)

window.mainloop()
