FROM python:3-buster

EXPOSE 80
WORKDIR /iotapp

COPY . /iotapp
RUN pip3 install -r requirements.txt

ENV FLASK_APP iotapp.py
ENV FLASK_RUN_HOST 0.0.0.0

CMD ["flask", "run"]
