FROM oven/bun:1.3.14 AS bun
FROM node:22-trixie-slim
COPY --from=bun /usr/local/bin/bun /usr/local/bin/bun
ENV PATH="/root/.bun/bin:${PATH}"

RUN apt-get update \
    && apt-get install -y --no-install-recommends openjdk-21-jre-headless \
    && rm -rf /var/lib/apt/lists/*

RUN bun install --global firebase-tools@15.0.0

WORKDIR /workspace
COPY firebase.json ./
COPY firebase/ ./firebase/

# Baixa o binário do emulador Firestore durante o build da imagem.
RUN firebase setup:emulators:firestore

EXPOSE 8080 9099
CMD ["firebase", "emulators:start", "--project", "demo-bcc323", "--only", "auth,firestore"]
