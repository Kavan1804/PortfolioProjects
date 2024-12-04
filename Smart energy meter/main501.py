import pyrebase
import os,time
import numpy as np # linear algebra
import pandas as pd # data processing, CSV file I/O 
from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestRegressor
import pandas as pd
from sklearn.neighbors import KNeighborsClassifier
from sklearn.model_selection import train_test_split

config = {
  "apiKey": "VEZSWYxOsbChZu8B7mkpghm2prMYvEESoUNz6LCv",
  "authDomain": "smart-energy-meter-e48aa-default-rtdb.firebaseio.com",
  "databaseURL": "https://smart-energy-meter-e48aa-default-rtdb.firebaseio.com",
  "storageBucket": "smart-energy-meter-e48aa-default"
}

firebase = pyrebase.initialize_app(config)
db = firebase.database()
print(db)

ad=pd.read_csv("./utility_data_final.csv")
ad.shape
ad.info()
ad.describe()
ad.head()
print(ad)
y = ad['Consumption Group'].values
X = ad.drop('Consumption Group', axis=1).values 
knn = KNeighborsClassifier(n_neighbors=6)
knn.fit(X,y)

values = db.child("TKWH").get()
m=values.val()
print(m)

values = db.child("waterflow").get()
n=values.val()
print(n)

      
values = db.child("Gasflow").get()
r=values.val()
print(r)


x_new = [m,n,r]
new_pred = knn.predict([x_new])
type(new_pred)
print("Prediction : {}".format(new_pred))

db.update({"Result":format(new_pred)})
time.sleep(1)
