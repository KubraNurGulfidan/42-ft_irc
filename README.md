# ft_irc - Internet Relay Chat Server

## Proje Hakkında (TR)
Bu proje, 42 eğitim programı kapsamında, C++98 standartları kullanılarak geliştirilmiş bir IRC sunucusudur. Gerçek bir IRC istemcisi (örneğin HexChat veya Irssi) ile iletişim kurabilen, çoklu istemci desteğine sahip ve RFC standartlarına uygun bir sunucu mimarisi hedeflenmiştir.

### Teknik Özellikler
- **Socket Programming**: TCP/IP protokolü üzerinden düşük seviyeli ağ iletişimi.
- **I/O Multiplexing**: poll() (veya benzeri) fonksiyonu kullanılarak tek bir işlem parçacığında (thread) eşzamanlı olarak birden fazla istemcinin yönetilmesi.
- **Protokol Yönetimi**: IRC mesaj yapısının (Prefix, Command, Params) ayrıştırılması ve işlenmesi.
- **Kanal ve Kullanıcı Yönetimi**: Kullanıcı kayıt, kanal oluşturma, operatör yetkileri ve mesaj iletimi.

### Desteklenen Komutlar
- **PASS, NICK, USER**: Kayıt ve kimlik doğrulama.
- **JOIN, PART, QUIT**: Kanal yönetimi ve sunucudan ayrılma.
- **PRIVMSG, NOTICE**: Özel ve genel mesaj gönderimi.
- **KICK, INVITE, TOPIC, MODE**: Kanal operatörü işlemleri.

---

## About the Project (EN)
This project is an IRC server developed using C++98 as part of the 42 curriculum. The goal was to build a server architecture compliant with RFC standards, capable of handling multiple clients simultaneously and communicating with real IRC clients like HexChat or Irssi.

### Technical Features
- **Socket Programming**: Low-level network communication via the TCP/IP protocol.
- **I/O Multiplexing**: Managing multiple clients concurrently in a single thread using the poll() function (or equivalent).
- **Protocol Management**: Parsing and processing the IRC message structure (Prefix, Command, Params).
- **Channel & User Management**: Handling user registration, channel creation, operator privileges, and message routing.

### Supported Commands
- **PASS, NICK, USER**: Registration and authentication.
- **JOIN, PART, QUIT**: Channel management and disconnection.
- **PRIVMSG, NOTICE**: Private and public messaging.
- **KICK, INVITE, TOPIC, MODE**: Channel operator actions.

---

## Kurulum ve Çalıştırma / Installation & Usage
1. **Derleme (Compilation)**
   ```bash
   make
2. **Başlatma (Running)**
   ```bash
   ./ircserv <port> <password>
**Örnek / Example:**
   ./ircserv 6667 my_secret_pass

---

## Proje Yapısı / Project Structure
server/: Sunucu çekirdek mantığı ve ağ döngüsü.
client/: İstemci verileri ve durum yönetimi.
