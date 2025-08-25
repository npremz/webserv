# 🌐 Webserv - HTTP/1.1 Server Implementation

A lightweight, high-performance HTTP/1.1 server written in C++ as part of the 42 School curriculum. This project implements a fully functional web server from scratch, handling multiple clients simultaneously using epoll for efficient I/O multiplexing.

## ✨ Features

- **HTTP/1.1 Compliant**: Full support for GET, POST, and DELETE methods
- **Non-blocking I/O**: Efficient handling of multiple concurrent connections using epoll
- **CGI Support**: Execute PHP and Python scripts dynamically
- **Virtual Hosts**: Host multiple websites on a single server
- **Custom Error Pages**: Define custom error pages for different HTTP status codes
- **File Upload**: Support for multipart/form-data uploads
- **Directory Listing**: Auto-index feature for directory browsing
- **Request Body Size Limiting**: Configurable client body size limits
- **Chunked Transfer Encoding**: Support for chunked request bodies

## 🚀 Quick Start

### Prerequisites
- C++ compiler with C++98 support
- Make
- Linux/Unix environment (epoll support required)

### Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/webserv.git
cd webserv

# Compile the server
make

# Run with default configuration
./webserv

# Run with custom configuration
./webserv path/to/config.conf
```

## 📋 Configuration

The server behavior is controlled through configuration files using an nginx-like syntax.

### Basic Configuration Example

```nginx
server {
    listen 8080;
    server_name localhost;
    
    root ./www;
    index index.html index.htm;
    
    allow_methods GET POST DELETE;
    client_max_body_size 10M;
    
    error_page 404 /errors/404.html;
    error_page 500 502 503 504 /errors/50x.html;
    
    location / {
        autoindex on;
    }
    
    location /upload {
        allow_methods POST;
        upload_enable on;
        upload_path /uploads;
    }
    
    location /api {
        allow_methods GET POST;
        cgi_extension .php;
        cgi_pass /usr/bin/php-cgi;
    }
}
```

### Configuration Directives

#### Server Block Directives

| Directive | Description | Example |
|-----------|-------------|---------|
| `listen` | IP:port or port to listen on | `listen 127.0.0.1:8080;` or `listen 8080;` |
| `server_name` | Virtual host names | `server_name example.com www.example.com;` |
| `root` | Document root directory | `root /var/www/html;` |
| `index` | Default index files | `index index.html index.php;` |
| `allow_methods` | Allowed HTTP methods | `allow_methods GET POST DELETE;` |
| `client_max_body_size` | Maximum request body size | `client_max_body_size 5M;` |
| `error_page` | Custom error pages | `error_page 404 /404.html;` |
| `autoindex` | Enable directory listing | `autoindex on;` |

#### Location Block Directives

Location blocks inherit directives from their parent server block but can override them.

| Directive | Description | Example |
|-----------|-------------|---------|
| `location` | URI path to match | `location /api { ... }` |
| `allow_methods` | Override allowed methods | `allow_methods GET;` |
| `root` | Override document root | `root /var/www/api;` |
| `autoindex` | Directory listing for location | `autoindex off;` |
| `index` | Location-specific index files | `index api.php;` |
| `upload_enable` | Enable file uploads | `upload_enable on;` |
| `upload_path` | Upload directory path | `upload_path /uploads;` |
| `cgi_extension` | CGI script extension | `cgi_extension .php;` |
| `cgi_pass` | CGI interpreter path | `cgi_pass /usr/bin/php-cgi;` |
| `return` | HTTP redirect | `return 301 https://example.com;` |
| `client_max_body_size` | Override max body size | `client_max_body_size 100M;` |

### Advanced Configuration Examples

#### Multiple Virtual Hosts

```nginx
# First virtual host
server {
    listen 80;
    server_name example.com www.example.com;
    root /var/www/example;
    
    location / {
        index index.html;
    }
}

# Second virtual host
server {
    listen 80;
    server_name api.example.com;
    root /var/www/api;
    
    location / {
        allow_methods GET POST;
        cgi_extension .php;
        cgi_pass /usr/bin/php-cgi;
    }
}
```

#### CGI Configuration

```nginx
server {
    listen 8080;
    
    # PHP scripts
    location /php {
        root /var/www;
        cgi_extension .php;
        cgi_pass /usr/bin/php-cgi;
    }
    
    # Python scripts
    location /python {
        root /var/www;
        cgi_extension .py;
        cgi_pass /usr/bin/python3;
    }
}
```

#### Upload Configuration

```nginx
server {
    listen 8080;
    
    location /upload {
        allow_methods POST;
        upload_enable on;
        upload_path /storage;
        client_max_body_size 50M;
    }
}
```

#### Redirection

```nginx
server {
    listen 80;
    server_name old-domain.com;
    
    location / {
        return 301 https://new-domain.com;
    }
}
```

### Configuration Rules

1. **Comments**: Use `#` for single-line comments
2. **Blocks**: Server and location blocks must be enclosed in `{}`
3. **Semicolons**: All directives must end with `;`
4. **Paths**: Relative paths are relative to the server's working directory
5. **Size Units**: Supported units for sizes are `K`/`k` (kilobytes) and `M`/`m` (megabytes)
6. **Multiple Values**: Some directives accept multiple values (e.g., `server_name`, `index`)

## 🔧 Technical Details

- **Language**: C++98
- **I/O Model**: Non-blocking with epoll
- **Architecture**: Event-driven
- **CGI**: Fork-exec model with pipe communication
- **Parser**: Custom HTTP/1.1 parser with chunked encoding support

## 📝 License

This project is part of the 42 School curriculum.

## 👥 Authors

- Nicolas Prémont (npremont)
- [Other contributors]

## 🙏 Acknowledgments

- 42 School for the project subject
- The nginx documentation for configuration syntax inspiration
- RFC 2616 (HTTP/1.1 specification)

---

*For more information about 42 School projects, visit [42.fr](https://42.fr)*