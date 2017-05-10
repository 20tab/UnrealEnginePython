The HTTP API
-

The IHttpRequest and IHttpResponse interfaces are exposed as Python classes (both inheriting from IHttpBase)

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
