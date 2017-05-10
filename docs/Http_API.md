The HTTP API
-

The IHttpRequest and IHttpResponse interfaces are exposed as Python classes (both inheriting from IHttpBase)

The usage is pretty 'pythonic' and support both polling and evented approaches:

```python
from unreal_engine import IHttpRequest
import json

request = IHttpRequest('Get', 'http://httpbin.org/user-agent')
request.process_request()

response = request.get_response()

data = json.loads(response.get_content_as_string())

ue.log(data)
```
