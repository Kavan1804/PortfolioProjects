import os
import requests
from datetime import datetime

class FlightSearch():
    def __init__(self):
        self.flight_endpoint = "https://test.api.amadeus.com/v2/shopping/flight-offers"
        self.iata_endpoint = "https://test.api.amadeus.com/v1/reference-data/locations/cities"
        self._token_endpoint = "https://test.api.amadeus.com/v1/security/oauth2/token"
        self._api_key = os.environ.get("FLIGHT_API_KEY")
        self._api_secret = os.environ.get("FLIGHT_API_SECRET")
        self._token = self._get_new_token()

    # def iata_code(self, city_name):
    #     code = "testing"
    #     return code

    def _get_new_token(self):
        header = {
            'Content-Type': 'application/x-www-form-urlencoded'
        }
        body = {
            'grant_type': 'client_credentials',
            'client_id': self._api_key,
            'client_secret': self._api_secret
        }
        response = requests.post(url=self._token_endpoint, headers=header, data=body)
        return response.json()['access_token']

    def get_destination_code(self, city_name):
        headers = {"Authorization": f"Bearer {self._token}"}
        query = {
            "keyword": city_name,
            "max": "2",
            "include": "AIRPORTS",
        }
        response = requests.get(
            url=self.iata_endpoint,
            headers=headers,
            params=query
        )
        try:
            code = response.json()["data"][0]['iataCode']
        except IndexError:
            print(f"IndexError: No airport code found for {city_name}.")
            return "N/A"
        except KeyError:
            print(f"KeyError: No airport code found for {city_name}.")
            return "Not Found"

        return code

    def check_flights(self, citylocation, destlocation, fromdate, todate, is_direct=True):
        header = {
            "Authorization": f"Bearer {self._token}"
        }

        parameters = {
            "originLocationCode": citylocation,
            "destinationLocationCode": destlocation,
            "departureDate": fromdate.strftime("%Y-%m-%d"),
            "returnDate": todate.strftime("%Y-%m-%d"),
            "adults": 1,
            "nonStop": "true" if is_direct else "false",
            "currencyCode": "INR",
            "max": 5

        }
        response = requests.get(url=self.flight_endpoint, params=parameters, headers=header)
        if response.status_code != 200:
            print(f"check_flights() response code: {response.status_code}")
            print("There was a problem with the flight search.\n"
                  "For details on status codes, check the API documentation:\n"
                  "https://developers.amadeus.com/self-service/category/flights/api-doc/flight-offers-search/api"
                  "-reference")
            print("Response body:", response.text)
            return None
        return response.json()

