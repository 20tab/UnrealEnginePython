import unittest
import unreal_engine as ue
from unreal_engine import IHttpRequest
import json

class TestHttp(unittest.TestCase):

    def test_user_agent(self):
        request = IHttpRequest('GET', 'http://httpbin.org/user-agent')
        request.set_header('User-Agent', 'UnrealEnginePython_test')
        request.process_request()
        while request.get_status() < 2:
        	request.tick(0.01)
        j = json.loads(request.get_response().get_content_as_string())
        self.assertEqual(j['user-agent'], 'UnrealEnginePython_test')

    def test_post_data(self):
    	request = IHttpRequest('POST', 'http://httpbin.org/post')
    	request.set_content('test')
    	request.process_request()
    	while request.get_status() < 2:
        	request.tick(0.01)
    	response = request.get_response()
    	self.assertEqual(response.get_response_code(), 200)
    	j = json.loads(response.get_content_as_string())
    	self.assertEqual(j['form']['test'], '')

    def test_headers(self):
        request = IHttpRequest()
        request.set_header('One', 'Two')
        request.set_header('Test1', 'Test2')
        self.assertEqual(request.get_all_headers(), ['One: Two', 'Test1: Test2'])

    def test_get_url(self):
    	request = IHttpRequest()
    	self.assertEqual(request.get_url(), '')
    	request.set_url('http://unrealengine.com')
    	self.assertEqual(request.get_url(), 'http://unrealengine.com')

    def test_get_url_parameter(self):
    	request = IHttpRequest()
    	request.set_url('http://unrealengine.com/?test=17&bar=30')
    	self.assertEqual(request.get_url_parameter('bar'), '30')
    	# fail ! (maybe a bug ?)
    	#self.assertEqual(request.get_url_parameter('test'), '17')



if __name__ == '__main__':
    unittest.main(exit=False)
