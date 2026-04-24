# ESP Trappe Interface Simulator
# PowerShell-based web server for local testing

param(
    [int]$Port = 8080
)

# Mock data
$mockConfig = @{
    datetime = "Sat Apr 19 12:00:00 2026"
    version = "3.0"
    uptime = "1 day, 2 hours"
    build_date = "Apr 19 2026 10:30:00"
    ssid = "MyWiFi"
    password = "password123"
    routerIP = "192.168.1.1"
    IP = "192.168.1.100"
    moduleName = "Trappe Simulator"
    mdns = "trappe-sim"
    mdnsAP = "trappe-sim-ap"
    publicKey = "mock_public_key"
    privateKey = "mock_private_key"
    dynamicIP = 0
} | ConvertTo-Json

$mockParams = @{
    module = @{
        nom = "Trappe Simulator"
        version = "3.0"
        build_date = "Apr 19 2026 10:30:00"
        uptime = "1 day, 2 hours"
    }
    setting = @{
        timeUpSec = "30"
        timeDownSec = "30"
        maxPowerAmp = "5.0"
        minPowerAmp = "1.5"
        maxAmpDownHori = "2.5"
        maxAmpDownVert = "2.5"
        maxAmpUpHori = "2.5"
        maxAmpUpVert = "2.5"
        delayIntensity = "1000"
        managementMode = "0"
    }
} | ConvertTo-Json

# Create HTTP listener
$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add("http://localhost:$Port/")
$listener.Start()

Write-Host "ESP Trappe Simulator running on http://localhost:$Port"
Write-Host "Press Ctrl+C to stop"

try {
    while ($listener.IsListening) {
        $context = $listener.GetContext()
        $request = $context.Request
        $response = $context.Response

        $url = $request.Url.AbsolutePath
        Write-Host "Request: $($request.HttpMethod) $url"

        $content = ""
        $contentType = "text/html"

        switch ($url) {
            "/jsonParam" {
                $content = $mockParams
                $contentType = "application/json"
            }
            "/jsonSetup" {
                $content = $mockConfig
                $contentType = "application/json"
            }
            "/setParam" {
                $query = [System.Web.HttpUtility]::ParseQueryString($request.Url.Query)
                Write-Host "Mock setting parameter: $($query.ToString())"
                $content = '{"status": "ok"}'
                $contentType = "application/json"
            }
            "/saveParam" {
                Write-Host "Mock saving configuration"
                $content = '{"status": "saved"}'
                $contentType = "application/json"
            }
            "/setSetup" {
                $query = [System.Web.HttpUtility]::ParseQueryString($request.Url.Query)
                Write-Host "Mock setup update: $($query.ToString())"
                $content = '{"status": "setup saved"}'
                $contentType = "application/json"
            }
            {$_ -in "/reset", "/restart", "/firmware", "/setup", "/fileSetup"} {
                Write-Host "Mock action: $url"
                $content = "<h1>Mock $($url.TrimStart('/')) page</h1><p>This is a simulation.</p><a href='/'>Back to main</a>"
            }
            "/" {
                $content = Get-Content "data/index.html" -Raw
            }
            "/setup" {
                $content = Get-Content "data/setup.html" -Raw
            }
            "/firmware" {
                $content = Get-Content "data/firmware.html" -Raw
            }
            "/help" {
                $content = Get-Content "data/help.html" -Raw
            }
            "/style.css" {
                $content = Get-Content "data/style.css" -Raw
                $contentType = "text/css"
            }
            "/jquery.min.js" {
                $content = Get-Content "data/jquery.min.js" -Raw
                $contentType = "application/javascript"
            }
            default {
                $filePath = Join-Path "data" $url.TrimStart("/")
                if (Test-Path $filePath) {
                    $content = Get-Content $filePath -Raw
                    if ($filePath.EndsWith(".css")) { $contentType = "text/css" }
                    elseif ($filePath.EndsWith(".js")) { $contentType = "application/javascript" }
                } else {
                    $response.StatusCode = 404
                    $content = "File not found: $url"
                }
            }
        }

        $response.ContentType = $contentType
        $response.Headers.Add("Access-Control-Allow-Origin", "*")

        $buffer = [System.Text.Encoding]::UTF8.GetBytes($content)
        $response.ContentLength64 = $buffer.Length
        $response.OutputStream.Write($buffer, 0, $buffer.Length)
        $response.OutputStream.Close()
    }
}
catch {
    Write-Host "Server stopped: $($_.Exception.Message)"
}
finally {
    $listener.Stop()
    $listener.Close()
}