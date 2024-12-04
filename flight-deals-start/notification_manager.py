import os
from twilio.rest import Client
import smtplib


class NotificationManager:

    def __init__(self):
        self.my_email = "kavansiddesh1804@gmail.com"
        self.password = "lyyjyklcosubyjqf"
        self.client = Client(os.environ.get('TWILIO_SID'), os.environ.get("TWILIO_AUTH_TOKEN"))
        self.connection = smtplib.SMTP("smtp.gmail.com", port=587)
    def send_sms(self, message_body):
        message = self.client.messages.create(
            from_=os.environ.get("TWILIO_VIRTUAL_NUMBER"),
            body=message_body,
            to=os.environ.get("6363599416")
        )
        # Prints if successfully sent.
        print(message.sid)

    # Is SMS not working for you or prefer whatsapp? Connect to the WhatsApp Sandbox!
    # https://console.twilio.com/us1/develop/sms/try-it-out/whatsapp-learn
    # def send_whatsapp(self, message_body):
    #     message = self.client.messages.create(
    #         from_=f'whatsapp:{os.environ["TWILIO_WHATSAPP_NUMBER"]}',
    #         body=message_body,
    #         to=f'whatsapp:{os.environ["TWILIO_VERIFIED_NUMBER"]}'
    #     )
    #     print(message.sid)
    def send_emails(self, email_list, email_body):
        with self.connection:
            self.connection.starttls()
            self.connection.login(self.my_email, self.password)
            for email in email_list:
                self.connection.sendmail(
                    from_addr=self.my_email,
                    to_addrs=email,
                    msg=f"Subject:New Low Price Flight!\n\n{email_body}".encode('utf-8')
                )