# DevOps Tabanlı Akıllı Sera İzleme Sistemi

Bu proje, bir IoT cihazından (ESP32) gelen sensör verilerinin gerçek zamanlı izlenmesini ve bu mimarinin **Kubernetes-Ready (K8s-Native)** prensiplerine uygun, ölçeklenebilir bir DevOps döngüsüyle yönetilmesini hedefleyen bir bitirme projesidir.

##  Mimari ve Teknolojiler

- **Donanım:** ESP32 (IoT Edge Device)
- **Haberleşme:** MQTT Protokolü
- **Uygulama Katmanı:** ThingsBoard (IoT Platform)
- **Orkestrasyon & Dağıtım:** Docker Compose (Kubernetes hazırlık evresi)
- **Monitoring (Observability):** - Prometheus (Metrik toplama)
    - Grafana (Görselleştirme)
    - Loki & Promtail (Merkezi Log Yönetimi)
- **Otomasyon (CI/CD):** GitHub Actions (GitOps prensiplerine uygun)
- **Bulut/Altyapı:** Hetzner VPS (Ubuntu 22.04 LTS)

##  Projede Öne Çıkanlar

### 1. Kubernetes-Ready Tasarım (K8s-Native Architecture)
Proje, mikroservis mimarisiyle izole edilmiş konteynerlar üzerine inşa edilmiştir. Servisler arası iletişim ve konfigürasyon yapısı, gelecekteki **Kubernetes Cluster** geçişine uygun olacak şekilde; "stateless" (durumsuz) prensipler ve merkezi konfigürasyon yönetimi gözetilerek kurgulanmıştır.

### 2. Ölçeklenebilir Observability (Gözlemlenebilirlik)
Sistem, gerçek bir üretim ortamı (production) gibi izlenmektedir:
- **Prometheus/Grafana:** Altyapı metriklerinin (CPU, RAM, Uptime) anlık analizi.
- **Loki/Promtail:** Docker konteyner loglarının merkezi toplanması ve hata ayıklama süreçlerinin otomasyonu.

### 3. Otomatize Deployment (CI/CD Pipeline)
**GitHub Actions** ile entegre bir CI/CD hattı kurgulanmıştır:
- Kod değişiklikleri, otomatik test ve build süreçlerinden geçerek sunucuya güvenli (SSH) şekilde dağıtılır.
- "Infrastructure as Code" yaklaşımı ile tüm servisler `docker-compose` manifest dosyalarıyla standartlaştırılmıştır.

### 4. Güvenlik ve İzolasyon
- **Network Policy:** Konteynerlar arası ağ iletişimi 'internal network' ile izole edilmiştir.
- **Güvenli Erişim:** Pipeline süreçlerinde şifre yerine **SSH Key-Pair** tabanlı yetkilendirme kullanılmıştır.

---

##  Kurulum ve Dağıtım

```bash
# Projeyi klonlayın
git clone <repo-url>

# Servisleri ayağa kaldırın
sudo docker compose up -d
