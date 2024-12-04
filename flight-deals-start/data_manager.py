import requests
import os
from requests.auth import HTTPBasicAuth
from pprint import pprint

class DataManager:
    def __init__(self):
        self.bearer_token = os.environ.get("BEARER_TOKEN")
        self.bearer_headers = {
            "Authorization": f"Bearer {self.bearer_token}"
        }
        self.sheety_endpoint = os.environ.get("SHEETY_ENDPOINT")
        self.users_endpoint = os.environ.get("USERS_ENDPOINT")
        # self.sheety_username = os.environ.get("SHEETY_USERNAME")
        # self.sheety_password = os.environ.get("SHEETY_PASSWORD")
        # self.authorization = HTTPBasicAuth(self.sheety_username, self.sheety_password)

    def price_data(self):
        response = requests.get(url=self.sheety_endpoint, headers=self.bearer_headers)
        data = response.json()
        self.destination_data = data["prices"]
        return self.destination_data

    def update_destination_codes(self):
        for row in self.destination_data:
            new_data = {
                "price": {
                    "iataCode": row["iataCode"]
                }
            }
            response = requests.put(url=f"{self.sheety_endpoint}/{row["id"]}", json=new_data, headers=self.bearer_headers)

    def get_customer_emails(self):
        response = requests.get(url=self.users_endpoint, headers=self.bearer_headers)
        user_data = response.json()
        self.users = user_data["users"]
        return self.users