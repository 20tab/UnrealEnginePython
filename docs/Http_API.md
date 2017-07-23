The HTTP API
=

The IHttpRequest and IHttpResponse interfaces are exposed as Python classes (both inheriting from IHttpBase)

Unit tests available here: https://github.com/20tab/UnrealEnginePython/blob/master/tests/test_http.py

The usage is pretty 'pythonic' and support both polling and evented approaches:

```python
from unreal_engine import IHttpRequest
import json

request = IHttpRequest('GET', 'http://httpbin.org/user-agent')

def response_received(request, response, success):
    data = json.loads(response.get_content_as_string())
    ue.log(data['user-agent'])

# bind OnProcessRequestComplete event to the response_received callable
request.bind_on_process_request_complete(response_received)

# run the request
request.process_request()
```

If you prefer the polling apparoach, you need to check for the get_status() return value (an int). In the C++ it is defined as:

```c++
namespace EHttpRequestStatus
{
    enum Type
    {
        NotStarted,
        Processing,
        Failed,
        Failed_ConnectionError,
        Succeeded,
    }
}
```

so any value >= 2 will be a sign of request completion:

```python
from unreal_engine import IHttpRequest
import json

request = IHttpRequest('GET', 'http://httpbin.org/user-agent')

# run the request
request.process_request()

# check its status

if request.get_status() >= 2:
    response = request.get_response()
    data = json.loads(response.get_content_as_string())
    ue.log(data['user-agent'])
```

If you want to call request.get_status() in a loop  ensure to 'tick' it (otherwise you will block forever as python will never give back control to unreal):

```python
from unreal_engine import IHttpRequest
import json

request = IHttpRequest('GET', 'http://httpbin.org/user-agent')

# run the request
request.process_request()

# check its status
# tick it at each iteration to avoid blocking ue forever !
# the argument of tick is the DeltaSeconds added to the value returned by get_elapsed_time()
while request.get_status() < 2:
    request.tick(0.01)
    
response = request.get_response()
data = json.loads(response.get_content_as_string())
ue.log(data['user-agent'])
```

In case of long running requests, another event can be bound:

```python
from unreal_engine import IHttpRequest
import json

request = IHttpRequest('GET', 'http://httpbin.org/user-agent')

def response_received(request, response, success):
    data = json.loads(response.get_content_as_string())
    ue.log(data['user-agent'])
    
def request_progress(request, sent, received):
    ue.log(sent)
    ue.log(received)

# bind OnProcessRequestComplete event to the response_received callable
request.bind_on_process_request_complete(response_received)

request.bind_on_request_progress(request_progress)

# run the request
request.process_request()
```

Exposed methods for IHttpBase (inherited by IHttpRequest and IHttpResponse)
-


### get_content()

returns the payload of the request/response

### get_all_headers()

returns a the list of headers as a list of string, with each item as 'HeaderKey: HeaderValue'

### get_content_length()

returns the request/response body length

### get_content_type()

returns the request/response body content type

### get_header(name)

returns the value of the specified request/response header

### get_url()

returns the url

### get_url_parameter(name)

get the value of a url query string item


Exposed methods for IHttpRequest
-

### bind_on_process_request_complete(callable)

Assign a python callable to the OnProcessRequestComplete event

### bind_on_request_progress(callable)

Assign a python callable to the OnRequestProcess event

### append_to_header(key, value)

append a new value to an already existent request header

### cancel_request()

cancel a running request

### get_elapsed_time()

get the amount of seconds the request took to complete (governed by tick)

### get_response()

get the IHttpResponse of the request (if available)

### get_status()

get the status of the request (see above for possible values)

### get_verb()

get the http METHOD of the request (if set)

### process_request()

run the request

### set_content(body)

set the request body (as string or bytes)

### set_header(key, value)

set a request header

### set_url(url)

assign the url to the request

### set_verb(http_method)

assign the http METHOD to the request

### tick(delta_seconds)

'tick' the http engine

Exposed methods for IHttpResponse
-

### get_response_code()

returns the response status code

### get_content_as_string()

returns the response body as a string
