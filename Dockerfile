FROM mambaorg/micromamba:1.3.1

COPY --chown=$MAMBA_USER:$MAMBA_USER env.yml /tmp/env.yml
RUN micromamba install -y -n base -f /tmp/env.yml && \
    micromamba clean --all --yes

WORKDIR /home/mambauser
RUN mkdir ./data
COPY --chown=$MAMBA_USER:$MAMBA_USER docker.ini docker.ini
COPY --chown=$MAMBA_USER:$MAMBA_USER map_of_fle.py map_of_fle.py