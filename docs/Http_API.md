The HTTP API
-

The IHttpRequest and IHttpResponse interfaces are exposed as Python classes (both inheriting from IHttpBase)

The usage is pretty 'pythonic' and support both polling and evented approaches:

```python
from unreal_engine import IHttpRequest
import json

request = IHttpRequest('Get', 'http://httpbin.org/user-agent')

def response_received(request, response, success):
    data = json.loads(response.get_content_as_string())
    ue.log(data)

# bind OnProcessRequestComplete event to the response_received callable
request.bind_on_process_request_complete(response_received)

# run the request
request.process_request()
```
