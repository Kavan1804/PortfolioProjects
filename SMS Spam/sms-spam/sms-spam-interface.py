import streamlit as st
import pickle
import string
import nltk
from nltk.corpus import stopwords
from nltk.stem.porter import PorterStemmer
import sklearn

ps = PorterStemmer()


def transform_text(text):
    text = text.lower()
    text = nltk.word_tokenize(text)

    y = []
    for i in text:
        if i.isalnum():
            y.append(i)
    text = y
    x = []

    for i in text:
        if i not in stopwords.words('english') and string.punctuation:
            x.append(i)

    text = x
    z = []

    for i in text:
        z.append(ps.stem(i))

    return " ".join(z)


tfidf = pickle.load(open('vectorizer.pkl', 'rb'))
model = pickle.load(open('model.pkl', 'rb'))

st.title("Email/SMS Spam Classifier")
input_sms = st.text_input("Enter the Message")
if st.button('Predict'):

    # preprocess
    transformed_text = transform_text(input_sms)
    # vectorize
    vector_input = tfidf.transform([transformed_text])
    # predict
    result = model.predict(vector_input)[0]
    # display
    if result == 1:
        st.header("Spam")
    else:
        st.header("Not Spam")
