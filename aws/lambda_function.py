import os
import boto3
import json
import time

print('Loading function')
table_name = os.environ.get('DYNAMO_TABLE_NAME')
dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table(table_name)

def respond(err, res=None):
    return {
        'statusCode': '400' if err else '200',
        'body': err.message if err else json.dumps(res),
        'headers': {
            'Content-Type': 'application/json',
        },
    }

def respondTEST(err, res=None):
    return {
        'statusCode': '400' if err else '200',
        'body': err if err else json.dumps(res),
        'headers': {
            'Content-Type': 'application/json',
        },
    }


def lambda_handler(event, context):
    #print("Received event: " + json.dumps(event, indent=2))
    print("BODY: " + event["body"])

    operations = {
        'DELETE': lambda dynamo, x: dynamo.delete_item(**x),
        'GET': lambda dynamo, x: dynamo.scan(**x),
        'POST': lambda dynamo, x: dynamo.put_item(**x),
        'PUT': lambda dynamo, x: dynamo.update_item(**x),
    }

    operation = event['httpMethod']

    if operation == 'POST':
        doPOST(event)
        return respond(None, 'This was a POST request.')
    elif operation == 'GET':
        doGET(event)
        return respond(None, 'This was a GET request.')
    else:
        return respondTEST(ValueError, 'Unsupported method')


    '''
    if operation in operations:
        payload = event['queryStringParameters'] if operation == 'GET' else json.loads(event['body'])
        return respond(None, operations[operation](dynamo, payload))
    else:
        return respond(ValueError('Unsupported method "{}"'.format(operation)))
    '''

def doPOST(event):
    print('Entering doPOST')
    operation = event['httpMethod']
    data = json.loads(event['body'])
    deviceid = data['device-id']
    fridgetemp = data['fridge-temp']
    freezertemp = data['freezer-temp']
    ambienttemp =  data['ambient-temp']
    batteryvoltage = data['battery-voltage']
    rssi = data['rssi']
    t = int(time.time())
    epochtime = t
    datestr = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(t))
    print('OPERATION: ' + operation)
    table.put_item(
       Item={
            'device-id': deviceid,
            'fridge-temp': fridgetemp,
            'freezer-temp': freezertemp,
            'ambient-temp': ambienttemp,
            'rssi': rssi,
            'battery-voltage': batteryvoltage,
            'epoch-time': epochtime,
            'date-time': datestr
        }
    )


def doGET(event):
    print('Entering doGET')
    operation = event['httpMethod']
    print('OPERATION: ' + operation)
